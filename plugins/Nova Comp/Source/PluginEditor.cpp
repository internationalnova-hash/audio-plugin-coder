#include "PluginEditor.h"
#include "BinaryData.h"

#include <cstring>

NovaCompAudioProcessorEditor::NovaCompAudioProcessorEditor (NovaCompAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    webView = std::make_unique<SinglePageBrowser> (createWebOptions (*this));

    levelAttachment = std::make_unique<juce::WebSliderParameterAttachment> (*processorRef.apvts.getParameter (ParameterIDs::level), levelRelay, nullptr);
    toneAttachment = std::make_unique<juce::WebSliderParameterAttachment> (*processorRef.apvts.getParameter (ParameterIDs::tone), toneRelay, nullptr);
    gainAttachment = std::make_unique<juce::WebSliderParameterAttachment> (*processorRef.apvts.getParameter (ParameterIDs::gain), gainRelay, nullptr);
    mixAttachment = std::make_unique<juce::WebSliderParameterAttachment> (*processorRef.apvts.getParameter (ParameterIDs::mix), mixRelay, nullptr);
    modeAttachment = std::make_unique<juce::WebSliderParameterAttachment> (*processorRef.apvts.getParameter (ParameterIDs::mode), modeRelay, nullptr);
    magicModeAttachment = std::make_unique<juce::WebSliderParameterAttachment> (*processorRef.apvts.getParameter (ParameterIDs::magicMode), magicModeRelay, nullptr);
    meterViewAttachment = std::make_unique<juce::WebSliderParameterAttachment> (*processorRef.apvts.getParameter (ParameterIDs::meterView), meterViewRelay, nullptr);

    addAndMakeVisible (*webView);

    const auto cacheBustedUrl = juce::WebBrowserComponent::getResourceProviderRoot()
                              + "/index.html?v=" + juce::String (juce::Time::getCurrentTime().toMilliseconds());
    webView->goToURL (cacheBustedUrl);

    setResizable (false, false);
    setSize (980, 620);
    startTimerHz (30);
}

NovaCompAudioProcessorEditor::~NovaCompAudioProcessorEditor()
{
    stopTimer();
}

void NovaCompAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB (236, 226, 213));
}

void NovaCompAudioProcessorEditor::resized()
{
    if (webView != nullptr)
        webView->setBounds (getLocalBounds());
}

void NovaCompAudioProcessorEditor::timerCallback()
{
    if (webView == nullptr || ! webView->isVisible())
        return;

    const auto output = processorRef.outputPeakLevel.load();
    const auto gainReduction = processorRef.gainReductionLevel.load();
    const auto isHot = processorRef.outputIsHot.load();

    const auto meterJs = "if (window.updateMeters) { window.updateMeters(" + juce::String (gainReduction, 3)
                       + ", " + juce::String (output, 3)
                       + ", " + juce::String (isHot ? "true" : "false") + "); }";

    webView->evaluateJavascript (meterJs);
}

juce::WebBrowserComponent::Options NovaCompAudioProcessorEditor::createWebOptions (NovaCompAudioProcessorEditor& editor)
{
    auto options = juce::WebBrowserComponent::Options {};

   #if JUCE_WINDOWS
    options = options.withBackend (juce::WebBrowserComponent::Options::Backend::webview2)
                     .withWinWebView2Options (
                         juce::WebBrowserComponent::Options::WinWebView2 {}
                             .withUserDataFolder (juce::File::getSpecialLocation (juce::File::tempDirectory)
                                 .getChildFile ("NovaComp")));
   #endif

    options = options.withNativeIntegrationEnabled()
                     .withResourceProvider ([&editor] (const juce::String& url)
                     {
                         return editor.getResource (url);
                     })
                     .withOptionsFrom (editor.levelRelay)
                     .withOptionsFrom (editor.toneRelay)
                     .withOptionsFrom (editor.gainRelay)
                     .withOptionsFrom (editor.mixRelay)
                     .withOptionsFrom (editor.modeRelay)
                     .withOptionsFrom (editor.magicModeRelay)
                     .withOptionsFrom (editor.meterViewRelay);

    return options;
}

std::optional<juce::WebBrowserComponent::Resource> NovaCompAudioProcessorEditor::getResource (const juce::String& url)
{
    auto makeResource = [] (const char* data, int size, const char* mime)
    {
        std::vector<std::byte> bytes (static_cast<size_t> (size));
        std::memcpy (bytes.data(), data, static_cast<size_t> (size));

        return juce::WebBrowserComponent::Resource {
            std::move (bytes),
            juce::String (mime)
        };
    };

    auto resourcePath = url.fromFirstOccurrenceOf (juce::WebBrowserComponent::getResourceProviderRoot(), false, false);
    resourcePath = resourcePath.upToFirstOccurrenceOf ("?", false, false);

    if (resourcePath.isEmpty() || resourcePath == "/")
        resourcePath = "/index.html";

    if (resourcePath == "/index.html")
        return makeResource (nova_comp_BinaryData::index_html,
                             nova_comp_BinaryData::index_htmlSize,
                             "text/html");

    return std::nullopt;
}
