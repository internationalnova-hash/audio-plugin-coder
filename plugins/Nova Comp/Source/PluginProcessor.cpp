#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterIDs.hpp"

#include <cmath>

namespace
{
    struct ModeProfile
    {
        float ratioMin;
        float ratioMax;
        float attackMs;
        float releaseFastMs;
        float releaseSlowMs;
        float kneeDb;
        float warmthScale;
        float magicScale;
    };

    ModeProfile getModeProfile (int modeIndex) noexcept
    {
        switch (juce::jlimit (0, 2, modeIndex))
        {
            case 1: return { 3.5f, 4.5f, 5.0f, 60.0f, 500.0f, 5.0f, 0.028f, 0.80f };
            case 2: return { 6.0f, 8.0f, 2.5f, 80.0f, 700.0f, 2.5f, 0.022f, 0.65f };
            default: return { 2.5f, 3.5f, 10.0f, 90.0f, 1300.0f, 8.0f, 0.032f, 1.00f };
        }
    }

    float computeGainReductionDb (float overDb, float ratio, float kneeDb) noexcept
    {
        if (overDb <= 0.0f)
            return 0.0f;

        const auto slope = 1.0f - (1.0f / juce::jmax (1.0f, ratio));

        if (kneeDb <= 0.0f)
            return slope * overDb;

        const auto kneeStart = -0.5f * kneeDb;
        const auto kneeEnd = 0.5f * kneeDb;

        if (overDb <= kneeStart)
            return 0.0f;

        if (overDb < kneeEnd)
        {
            const auto x = overDb - kneeStart;
            return slope * (x * x) / (2.0f * kneeDb);
        }

        return slope * overDb;
    }
}

NovaCompAudioProcessor::NovaCompAudioProcessor()
    : AudioProcessor (BusesProperties()
        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, juce::Identifier ("NovaComp"), createParameterLayout())
{
}

NovaCompAudioProcessor::~NovaCompAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout NovaCompAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ParameterIDs::level, 1 },
        "Compression",
        juce::NormalisableRange<float> (0.0f, 10.0f, 0.01f),
        4.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [] (float value, int) { return juce::String (value, 1); }));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ParameterIDs::tone, 1 },
        "Tone Shaping",
        juce::NormalisableRange<float> (0.0f, 10.0f, 0.01f),
        5.0f,
        juce::String(),
        juce::AudioProcessorParameter::genericParameter,
        [] (float value, int)
        {
            const auto tiltDb = -3.0f + ((value / 10.0f) * 6.0f);
            const auto sign = tiltDb >= 0.0f ? "+" : "";
            return sign + juce::String (tiltDb, 1);
        }));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ParameterIDs::gain, 1 },
        "Output",
        juce::NormalisableRange<float> (-12.0f, 12.0f, 0.01f),
        0.0f,
        "dB",
        juce::AudioProcessorParameter::genericParameter,
        [] (float value, int)
        {
            const auto sign = value >= 0.0f ? "+" : "";
            return sign + juce::String (value, 1) + " dB";
        }));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ParameterIDs::mix, 1 },
        "Mix",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f),
        70.0f,
        "%",
        juce::AudioProcessorParameter::genericParameter,
        [] (float value, int) { return juce::String (value, 0) + "%"; }));

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { ParameterIDs::mode, 1 },
        "Mode",
        juce::StringArray { "Smooth", "Punch", "Limit" },
        0));

    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { ParameterIDs::magicMode, 1 },
        "Magic",
        false));

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { ParameterIDs::meterView, 1 },
        "Meter View",
        juce::StringArray { "GR", "OUT" },
        0));

    return layout;
}

const juce::String NovaCompAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NovaCompAudioProcessor::acceptsMidi() const
{
    return false;
}

bool NovaCompAudioProcessor::producesMidi() const
{
    return false;
}

bool NovaCompAudioProcessor::isMidiEffect() const
{
    return false;
}

double NovaCompAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NovaCompAudioProcessor::getNumPrograms()
{
    return 1;
}

int NovaCompAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NovaCompAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String NovaCompAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void NovaCompAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void NovaCompAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate > 0.0 ? sampleRate : 44100.0;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = currentSampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32> (juce::jmax (1, getTotalNumOutputChannels()));

    toneLowShelf.prepare (spec);
    toneHighShelf.prepare (spec);
    topPolishFilter.prepare (spec);
    outputTrim.prepare (spec);
    outputTrim.setRampDurationSeconds (0.03);

    toneLowShelf.reset();
    toneHighShelf.reset();
    topPolishFilter.reset();
    outputTrim.reset();

    dryBuffer.setSize (juce::jmax (2, getTotalNumOutputChannels()), samplesPerBlock);
    dryBuffer.clear();

    detectorEnvelope = 0.0f;
    smoothedGainReductionDb = 0.0f;
    outputPeakHold = 0.0f;
    outputPeakLevel.store (0.0f);
    gainReductionLevel.store (0.0f);
    outputIsHot.store (false);
}

void NovaCompAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NovaCompAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto mainOutput = layouts.getMainOutputChannelSet();
    if (mainOutput != juce::AudioChannelSet::mono() && mainOutput != juce::AudioChannelSet::stereo())
        return false;

    return mainOutput == layouts.getMainInputChannelSet();
}
#endif

void NovaCompAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;

    const auto totalNumInputChannels = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto channel = totalNumInputChannels; channel < totalNumOutputChannels; ++channel)
        buffer.clear (channel, 0, buffer.getNumSamples());

    if (buffer.getNumSamples() == 0)
        return;

    const auto compression = apvts.getRawParameterValue (ParameterIDs::level)->load();
    const auto tone = apvts.getRawParameterValue (ParameterIDs::tone)->load();
    const auto outputGainDb = apvts.getRawParameterValue (ParameterIDs::gain)->load();
    const auto mix = apvts.getRawParameterValue (ParameterIDs::mix)->load();
    const auto modeIndex = static_cast<int> (apvts.getRawParameterValue (ParameterIDs::mode)->load());
    const auto magicOn = apvts.getRawParameterValue (ParameterIDs::magicMode)->load() > 0.5f;

    const auto compressionNorm = juce::jlimit (0.0f, 1.0f, compression / 10.0f);
    const auto toneNorm = juce::jlimit (0.0f, 1.0f, tone / 10.0f);
    const auto mixNorm = juce::jlimit (0.0f, 1.0f, mix / 100.0f);

    const auto mode = getModeProfile (modeIndex);
    const auto thresholdDb = -10.0f - (compressionNorm * 22.0f);
    const auto ratio = mode.ratioMin + (compressionNorm * (mode.ratioMax - mode.ratioMin));

    const auto tiltDb = -3.0f + (toneNorm * 6.0f);
    const auto lowGainDb = -tiltDb * 0.5f;
    const auto highGainDb = tiltDb * 0.5f;

    const auto grNormAtBlockStart = juce::jlimit (0.0f, 1.0f, smoothedGainReductionDb / 10.0f);
    const auto topSmoothAmount = (grNormAtBlockStart * 0.28f) + (magicOn ? 0.08f * mode.magicScale : 0.0f);
    const auto smoothingCutoff = juce::jlimit (3800.0f, 18000.0f,
                                               18000.0f - (topSmoothAmount * 14000.0f) - ((1.0f - toneNorm) * 700.0f));

    *toneLowShelf.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf (currentSampleRate, 220.0f, 0.70f, juce::Decibels::decibelsToGain (lowGainDb));
    *toneHighShelf.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf (currentSampleRate, 2800.0f, 0.70f, juce::Decibels::decibelsToGain (highGainDb));
    *topPolishFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass (currentSampleRate, smoothingCutoff, 0.75f);

    dryBuffer.setSize (buffer.getNumChannels(), buffer.getNumSamples(), false, false, true);
    dryBuffer.makeCopyOf (buffer, true);

    juce::dsp::AudioBlock<float> wetBlock (buffer);
    juce::dsp::ProcessContextReplacing<float> wetContext (wetBlock);

    toneLowShelf.process (wetContext);
    toneHighShelf.process (wetContext);

    const auto detectorAttackCoeff = std::exp (-1.0f / (0.001f * juce::jmax (1.0f, mode.attackMs) * static_cast<float> (currentSampleRate)));

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float detectorSample = 0.0f;
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
            detectorSample += std::abs (buffer.getSample (channel, sample));

        detectorSample /= static_cast<float> (juce::jmax (1, totalNumInputChannels));
        detectorSample *= 0.92f + (toneNorm * 0.16f) + (magicOn ? 0.03f * mode.magicScale : 0.0f);

        const auto grNorm = juce::jlimit (0.0f, 1.0f, smoothedGainReductionDb / 10.0f);
        const auto releaseMs = mode.releaseFastMs + (grNorm * (mode.releaseSlowMs - mode.releaseFastMs));
        const auto releaseScale = magicOn ? (1.10f + (0.05f * mode.magicScale)) : 1.0f;
        const auto detectorReleaseCoeff = std::exp (-1.0f / (0.001f * releaseMs * releaseScale * static_cast<float> (currentSampleRate)));

        if (detectorSample > detectorEnvelope)
            detectorEnvelope = (detectorAttackCoeff * detectorEnvelope) + ((1.0f - detectorAttackCoeff) * detectorSample);
        else
            detectorEnvelope = (detectorReleaseCoeff * detectorEnvelope) + ((1.0f - detectorReleaseCoeff) * detectorSample);

        const auto detectorDb = juce::Decibels::gainToDecibels (detectorEnvelope + 1.0e-6f, -60.0f);
        const auto overDb = detectorDb - thresholdDb;
        auto targetGrDb = computeGainReductionDb (overDb, ratio, mode.kneeDb);

        if (magicOn)
            targetGrDb *= 1.04f + (0.06f * mode.magicScale);

        const auto grAttackMs = juce::jmax (1.5f, mode.attackMs * (magicOn ? 0.92f : 1.0f));
        const auto grAttackCoeff = std::exp (-1.0f / (0.001f * grAttackMs * static_cast<float> (currentSampleRate)));
        const auto grReleaseCoeff = std::exp (-1.0f / (0.001f * releaseMs * releaseScale * static_cast<float> (currentSampleRate)));

        if (targetGrDb > smoothedGainReductionDb)
            smoothedGainReductionDb = (grAttackCoeff * smoothedGainReductionDb) + ((1.0f - grAttackCoeff) * targetGrDb);
        else
            smoothedGainReductionDb = (grReleaseCoeff * smoothedGainReductionDb) + ((1.0f - grReleaseCoeff) * targetGrDb);

        const auto compressionGain = juce::Decibels::decibelsToGain (-smoothedGainReductionDb);
        const auto warmthAmount = (smoothedGainReductionDb * mode.warmthScale)
                                + (compressionNorm * 0.04f)
                                + (magicOn ? 0.02f * mode.magicScale : 0.0f);
        const auto saturationBlend = juce::jlimit (0.0f, 1.0f,
                                                   0.16f + (compressionNorm * 0.18f) + (magicOn ? 0.05f : 0.0f));

        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* samples = buffer.getWritePointer (channel);
            const auto compressed = samples[sample] * compressionGain;
            const auto sweetened = std::tanh (compressed * (1.0f + warmthAmount));
            samples[sample] = juce::jmap (saturationBlend, compressed, sweetened);
        }
    }

    topPolishFilter.process (wetContext);

    if (totalNumInputChannels >= 2 && magicOn)
    {
        auto* left = buffer.getWritePointer (0);
        auto* right = buffer.getWritePointer (1);
        const auto widthBoost = 1.0f + (0.03f * mode.magicScale);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            const auto mid = 0.5f * (left[sample] + right[sample]);
            const auto side = 0.5f * (left[sample] - right[sample]) * widthBoost;
            left[sample] = mid + side;
            right[sample] = mid - side;
        }
    }

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* wet = buffer.getWritePointer (channel);
        const auto* dry = dryBuffer.getReadPointer (channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            wet[sample] = (dry[sample] * (1.0f - mixNorm)) + (wet[sample] * mixNorm);
    }

    const auto autoTrimDb = -(smoothedGainReductionDb * 0.15f) + (magicOn ? -0.35f : 0.0f);
    outputTrim.setGainDecibels (outputGainDb + autoTrimDb);
    outputTrim.process (wetContext);

    float peak = 0.0f;
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        const auto* samples = buffer.getReadPointer (channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            peak = juce::jmax (peak, std::abs (samples[sample]));
    }

    outputPeakHold = peak > outputPeakHold ? peak : outputPeakHold * 0.92f;
    outputPeakLevel.store (juce::jlimit (0.0f, 1.0f, outputPeakHold));
    gainReductionLevel.store (juce::jlimit (0.0f, 1.0f, smoothedGainReductionDb / 10.0f));
    outputIsHot.store (peak >= 0.98f);
}

bool NovaCompAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* NovaCompAudioProcessor::createEditor()
{
    return new NovaCompAudioProcessorEditor (*this);
}

void NovaCompAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    const auto state = apvts.copyState();
    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, destData);
}

void NovaCompAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xmlState = getXmlFromBinary (data, sizeInBytes))
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NovaCompAudioProcessor();
}
