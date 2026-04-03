#include "PluginEditor.h"
#include "BinaryData.h"

NovaSpaceAudioProcessorEditor::NovaSpaceAudioProcessorEditor (NovaSpaceAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    spaceAttachment = std::make_unique<juce::WebSliderParameterAttachment> (
        *audioProcessor.apvts.getParameter ("space"), spaceRelay);
    airAttachment = std::make_unique<juce::WebSliderParameterAttachment> (
        *audioProcessor.apvts.getParameter ("air"), airRelay);
    depthAttachment = std::make_unique<juce::WebSliderParameterAttachment> (
        *audioProcessor.apvts.getParameter ("depth"), depthRelay);
    widthAttachment = std::make_unique<juce::WebSliderParameterAttachment> (
        *audioProcessor.apvts.getParameter ("width"), widthRelay);
    mixAttachment = std::make_unique<juce::WebSliderParameterAttachment> (
        *audioProcessor.apvts.getParameter ("mix"), mixRelay);
    novaModeAttachment = std::make_unique<juce::WebSliderParameterAttachment> (
        *audioProcessor.apvts.getParameter ("nova_mode"), novaModeRelay);

    webView = std::make_unique<juce::WebBrowserComponent> (createWebViewOptions (*this));
    addAndMakeVisible (*webView);
    webView->goToURL (juce::WebBrowserComponent::getResourceProviderRoot());

    setSize (820, 560);
}

NovaSpaceAudioProcessorEditor::~NovaSpaceAudioProcessorEditor() = default;

void NovaSpaceAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xffefe6d8));
}

void NovaSpaceAudioProcessorEditor::resized()
{
    if (webView)
        webView->setBounds (getLocalBounds());
}

juce::WebBrowserComponent::Options NovaSpaceAudioProcessorEditor::createWebViewOptions (NovaSpaceAudioProcessorEditor& editor)
{
    auto options = juce::WebBrowserComponent::Options{}
        .withNativeIntegrationEnabled()
        .withResourceProvider ([&editor] (const auto& url) {
            return editor.getResource (url);
        })
        .withOptionsFrom (editor.spaceRelay)
        .withOptionsFrom (editor.airRelay)
        .withOptionsFrom (editor.depthRelay)
        .withOptionsFrom (editor.widthRelay)
        .withOptionsFrom (editor.mixRelay)
        .withOptionsFrom (editor.novaModeRelay);

   #if JUCE_WINDOWS
    options = options.withBackend (juce::WebBrowserComponent::Options::Backend::webview2)
                     .withWinWebView2Options (
                        juce::WebBrowserComponent::Options::WinWebView2{}
                            .withUserDataFolder (
                                juce::File::getSpecialLocation (juce::File::tempDirectory)
                                    .getChildFile ("Noizefield_NovaSpace")));
   #endif

    return options;
}

std::optional<juce::WebBrowserComponent::Resource> NovaSpaceAudioProcessorEditor::getResource (const juce::String& url)
{
    const auto urlToRetrieve = url == "/"
        ? juce::String { "index.html" }
        : url.fromFirstOccurrenceOf ("/", false, false);

    for (int index = 0; index < BinaryData::namedResourceListSize; ++index)
    {
        const auto* resourceName = BinaryData::namedResourceList[index];
        const auto* originalFilename = BinaryData::getNamedResourceOriginalFilename (resourceName);

        if (originalFilename == nullptr)
            continue;

        if (! juce::String (originalFilename).endsWith (urlToRetrieve))
            continue;

        int dataSize = 0;
        const auto* data = BinaryData::getNamedResource (resourceName, dataSize);

        if (data == nullptr || dataSize <= 0)
            continue;

        std::vector<std::byte> bytes (static_cast<size_t> (dataSize));
        std::memcpy (bytes.data(), data, static_cast<size_t> (dataSize));
        return juce::WebBrowserComponent::Resource {
            std::move (bytes),
            juce::String { getMimeForExtension (getExtension (urlToRetrieve).toLowerCase()) }
        };
    }

    return std::nullopt;
}

const char* NovaSpaceAudioProcessorEditor::getMimeForExtension (const juce::String& extension)
{
    static const std::unordered_map<juce::String, const char*> mimeMap {
        { "html", "text/html" },
        { "css", "text/css" },
        { "js", "text/javascript" },
        { "json", "application/json" },
        { "svg", "image/svg+xml" },
        { "png", "image/png" }
    };

    if (const auto it = mimeMap.find (extension); it != mimeMap.end())
        return it->second;

    return "text/plain";
}

juce::String NovaSpaceAudioProcessorEditor::getExtension (juce::String filename)
{
    return filename.fromLastOccurrenceOf (".", false, false);
}