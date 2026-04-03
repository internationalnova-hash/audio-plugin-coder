#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterIDs.hpp"

namespace
{
float remap (float value, float minValue, float maxValue)
{
    return juce::jmap (juce::jlimit (0.0f, 1.0f, value), minValue, maxValue);
}
}

NovaSpaceAudioProcessor::NovaSpaceAudioProcessor()
    : AudioProcessor (BusesProperties()
        .withInput ("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, juce::Identifier ("NovaSpaceParameters"), createParameterLayout())
{
}

NovaSpaceAudioProcessor::~NovaSpaceAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout NovaSpaceAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    auto percentText = [] (float value, int)
    {
        return juce::String (juce::roundToInt (value * 100.0f)) + " %";
    };

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ParameterIDs::space, 1 }, "Space",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.45f,
        juce::String(), juce::AudioProcessorParameter::genericParameter, percentText));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ParameterIDs::air, 1 }, "Air",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.60f,
        juce::String(), juce::AudioProcessorParameter::genericParameter, percentText));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ParameterIDs::depth, 1 }, "Depth",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.40f,
        juce::String(), juce::AudioProcessorParameter::genericParameter, percentText));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ParameterIDs::width, 1 }, "Width",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.80f,
        juce::String(), juce::AudioProcessorParameter::genericParameter, percentText));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ParameterIDs::mix, 1 }, "Mix",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.18f,
        juce::String(), juce::AudioProcessorParameter::genericParameter, percentText));

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { ParameterIDs::novaMode, 1 }, "Nova Mode",
        juce::StringArray { "Studio", "Arena", "Dream", "Vintage" }, 0));

    return layout;
}

const juce::String NovaSpaceAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NovaSpaceAudioProcessor::acceptsMidi() const
{
    return false;
}

bool NovaSpaceAudioProcessor::producesMidi() const
{
    return false;
}

bool NovaSpaceAudioProcessor::isMidiEffect() const
{
    return false;
}

double NovaSpaceAudioProcessor::getTailLengthSeconds() const
{
    return 4.0;
}

int NovaSpaceAudioProcessor::getNumPrograms()
{
    return 1;
}

int NovaSpaceAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NovaSpaceAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String NovaSpaceAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void NovaSpaceAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void NovaSpaceAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32> (juce::jmax (1, getTotalNumOutputChannels()));

    preDelayLeft.prepare (spec);
    preDelayRight.prepare (spec);
    preDelayLeft.reset();
    preDelayRight.reset();
    wetToneFilter.prepare (spec);
    wetToneFilter.reset();
    reverb.reset();

    wetBuffer.setSize (juce::jmax (2, getTotalNumOutputChannels()), samplesPerBlock);

    spaceSmoothed.reset (sampleRate, 0.08);
    airSmoothed.reset (sampleRate, 0.08);
    depthSmoothed.reset (sampleRate, 0.08);
    widthSmoothed.reset (sampleRate, 0.08);
    mixSmoothed.reset (sampleRate, 0.08);
    modeSmoothed.reset (sampleRate, 0.12);

    spaceSmoothed.setCurrentAndTargetValue (*apvts.getRawParameterValue (ParameterIDs::space));
    airSmoothed.setCurrentAndTargetValue (*apvts.getRawParameterValue (ParameterIDs::air));
    depthSmoothed.setCurrentAndTargetValue (*apvts.getRawParameterValue (ParameterIDs::depth));
    widthSmoothed.setCurrentAndTargetValue (*apvts.getRawParameterValue (ParameterIDs::width));
    mixSmoothed.setCurrentAndTargetValue (*apvts.getRawParameterValue (ParameterIDs::mix));
    modeSmoothed.setCurrentAndTargetValue (*apvts.getRawParameterValue (ParameterIDs::novaMode));

    updateWetFilter (airSmoothed.getCurrentValue(), juce::roundToInt (modeSmoothed.getCurrentValue()));
}

void NovaSpaceAudioProcessor::releaseResources()
{
    wetBuffer.setSize (0, 0);
}

bool NovaSpaceAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto output = layouts.getMainOutputChannelSet();
    const auto input = layouts.getMainInputChannelSet();

    if (output != juce::AudioChannelSet::mono() && output != juce::AudioChannelSet::stereo())
        return false;

    return input == output;
}

NovaSpaceAudioProcessor::ModeProfile NovaSpaceAudioProcessor::getModeProfile (int modeIndex)
{
    switch (juce::jlimit (0, 3, modeIndex))
    {
        case 0: return { 0.28f, 0.55f, 0.35f, 18.0f, 0.08f };
        case 1: return { 0.55f, 0.42f, 0.78f, 36.0f, 0.10f };
        case 2: return { 0.72f, 0.28f, 0.86f, 52.0f, 0.04f };
        case 3: return { 0.44f, 0.68f, 0.48f, 26.0f, 0.18f };
        default: break;
    }

    return { 0.28f, 0.55f, 0.35f, 18.0f, 0.08f };
}

void NovaSpaceAudioProcessor::updateWetFilter (float airValue, int modeIndex)
{
    const auto profile = getModeProfile (modeIndex);
    const auto cutoff = remap (airValue, 2200.0f, 18000.0f) * juce::jmap (profile.warmth, 1.0f, 0.78f);
    *wetToneFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass (
        currentSampleRate,
        juce::jlimit (400.0f, 20000.0f, cutoff));
}

void NovaSpaceAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;

    const auto totalNumInputChannels = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto channel = totalNumInputChannels; channel < totalNumOutputChannels; ++channel)
        buffer.clear (channel, 0, buffer.getNumSamples());

    if (buffer.getNumSamples() == 0)
        return;

    wetBuffer.setSize (juce::jmax (2, buffer.getNumChannels()), buffer.getNumSamples(), false, false, true);
    wetBuffer.makeCopyOf (buffer, true);

    spaceSmoothed.setTargetValue (*apvts.getRawParameterValue (ParameterIDs::space));
    airSmoothed.setTargetValue (*apvts.getRawParameterValue (ParameterIDs::air));
    depthSmoothed.setTargetValue (*apvts.getRawParameterValue (ParameterIDs::depth));
    widthSmoothed.setTargetValue (*apvts.getRawParameterValue (ParameterIDs::width));
    mixSmoothed.setTargetValue (*apvts.getRawParameterValue (ParameterIDs::mix));
    modeSmoothed.setTargetValue (*apvts.getRawParameterValue (ParameterIDs::novaMode));

    const auto modeValue = modeSmoothed.getNextValue();
    const auto modeIndex = juce::roundToInt (modeValue);
    const auto profile = getModeProfile (modeIndex);

    const auto space = spaceSmoothed.getNextValue();
    const auto air = airSmoothed.getNextValue();
    const auto depth = depthSmoothed.getNextValue();
    const auto width = widthSmoothed.getNextValue();
    const auto mix = mixSmoothed.getNextValue();

    const auto preDelayMs = profile.preDelayMs + remap (depth, 4.0f, 72.0f);
    const auto preDelaySamples = juce::jlimit (0.0f, 32760.0f, static_cast<float> (currentSampleRate * preDelayMs * 0.001f));

    preDelayLeft.setDelay (preDelaySamples);
    preDelayRight.setDelay (preDelaySamples + remap (width, 0.0f, 14.0f));

    updateWetFilter (air, modeIndex);

    juce::dsp::Reverb::Parameters params;
    params.roomSize = juce::jlimit (0.05f, 0.96f, 0.16f + profile.roomBias * 0.45f + space * 0.36f);
    params.damping = juce::jlimit (0.02f, 0.92f, profile.dampingBias - air * 0.52f + 0.18f);
    params.width = juce::jlimit (0.0f, 1.0f, width * 0.75f + profile.widthBias * 0.25f);
    params.wetLevel = juce::jlimit (0.0f, 1.0f, mix * (0.55f + depth * 0.35f));
    params.dryLevel = juce::jlimit (0.0f, 1.0f, 1.0f - mix * 0.85f);
    params.freezeMode = 0.0f;
    reverb.setParameters (params);

    auto* wetLeft = wetBuffer.getWritePointer (0);
    auto* wetRight = wetBuffer.getNumChannels() > 1 ? wetBuffer.getWritePointer (1) : wetBuffer.getWritePointer (0);

    for (int sample = 0; sample < wetBuffer.getNumSamples(); ++sample)
    {
        const auto delayedLeft = preDelayLeft.popSample (0);
        const auto delayedRight = preDelayRight.popSample (0);

        preDelayLeft.pushSample (0, wetLeft[sample]);
        preDelayRight.pushSample (0, wetRight[sample]);

        wetLeft[sample] = delayedLeft;
        wetRight[sample] = delayedRight;
    }

    juce::dsp::AudioBlock<float> reverbBlock (wetBuffer);
    juce::dsp::ProcessContextReplacing<float> reverbContext (reverbBlock);
    reverb.process (reverbContext);

    juce::dsp::AudioBlock<float> wetBlock (wetBuffer);
    juce::dsp::ProcessContextReplacing<float> wetContext (wetBlock);
    wetToneFilter.process (wetContext);

    const auto widthScale = juce::jlimit (0.0f, 1.4f, 0.3f + width * 1.1f);
    const auto wetTrim = 0.92f - profile.warmth * 0.12f;
    const auto warmthDrive = 1.0f + profile.warmth * 0.8f;

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        auto left = wetLeft[sample];
        auto right = wetRight[sample];

        const auto mid = 0.5f * (left + right);
        const auto side = 0.5f * (left - right) * widthScale;
        left = mid + side;
        right = mid - side;

        left = std::tanh (left * warmthDrive) * wetTrim;
        right = std::tanh (right * warmthDrive) * wetTrim;

        wetLeft[sample] = left;
        wetRight[sample] = right;
    }

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* dry = buffer.getWritePointer (channel);
        auto* wet = wetBuffer.getWritePointer (juce::jmin (channel, wetBuffer.getNumChannels() - 1));

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            dry[sample] = juce::jlimit (-1.0f, 1.0f, dry[sample] * params.dryLevel + wet[sample] * params.wetLevel);
    }
}

bool NovaSpaceAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* NovaSpaceAudioProcessor::createEditor()
{
    return new NovaSpaceAudioProcessorEditor (*this);
}

void NovaSpaceAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto state = apvts.copyState(); state.isValid())
    {
        if (auto xml = state.createXml())
            copyXmlToBinary (*xml, destData);
    }
}

void NovaSpaceAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));

    if (xml != nullptr && xml->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NovaSpaceAudioProcessor();
}