#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include "PluginProcessor.h"

class NovaSpaceAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit NovaSpaceAudioProcessorEditor (NovaSpaceAudioProcessor&);
    ~NovaSpaceAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    static juce::WebBrowserComponent::Options createWebViewOptions (NovaSpaceAudioProcessorEditor& editor);
    std::optional<juce::WebBrowserComponent::Resource> getResource (const juce::String& url);
    static const char* getMimeForExtension (const juce::String& extension);
    static juce::String getExtension (juce::String filename);

    NovaSpaceAudioProcessor& audioProcessor;

    // CRITICAL: Order must remain Relays -> WebView -> Attachments.
    juce::WebSliderRelay spaceRelay { "space" };
    juce::WebSliderRelay airRelay { "air" };
    juce::WebSliderRelay depthRelay { "depth" };
    juce::WebSliderRelay widthRelay { "width" };
    juce::WebSliderRelay mixRelay { "mix" };
    juce::WebSliderRelay novaModeRelay { "nova_mode" };

    std::unique_ptr<juce::WebBrowserComponent> webView;

    std::unique_ptr<juce::WebSliderParameterAttachment> spaceAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> airAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> depthAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> widthAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> mixAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> novaModeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NovaSpaceAudioProcessorEditor)
};