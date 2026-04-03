#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class NovaSpaceAudioProcessor : public juce::AudioProcessor
{
public:
    NovaSpaceAudioProcessor();
    ~NovaSpaceAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    struct ModeProfile
    {
        float roomBias;
        float dampingBias;
        float widthBias;
        float preDelayMs;
        float warmth;
    };

    using FilterType = juce::dsp::IIR::Filter<float>;
    using StereoFilter = juce::dsp::ProcessorDuplicator<FilterType, juce::dsp::IIR::Coefficients<float>>;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    static ModeProfile getModeProfile (int modeIndex);
    void updateWetFilter (float airValue, int modeIndex);

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> preDelayLeft { 32768 };
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> preDelayRight { 32768 };
    juce::dsp::Reverb reverb;
    StereoFilter wetToneFilter;

    juce::LinearSmoothedValue<float> spaceSmoothed;
    juce::LinearSmoothedValue<float> airSmoothed;
    juce::LinearSmoothedValue<float> depthSmoothed;
    juce::LinearSmoothedValue<float> widthSmoothed;
    juce::LinearSmoothedValue<float> mixSmoothed;
    juce::LinearSmoothedValue<float> modeSmoothed;

    juce::AudioBuffer<float> wetBuffer;
    double currentSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NovaSpaceAudioProcessor)
};