// This plugin was developed using Jan Wilczek's (WolfSound)
// Audio Plugin Template:
// https://github.com/JanWilczek/audio-plugin-template

#pragma once

namespace audio_plugin {

// Class for horseshoe-style knobs (continuous valued)
class HorseshoeLookAndFeel : public juce::LookAndFeel_V4 {
    public:
        void drawRotarySlider(juce::Graphics&, int, int, int, int, float, float, float, juce::Slider&) override;
};

// Class for filter type rotary switch
class FilterTypeLookAndFeel : public juce::LookAndFeel_V4 {
    public:
        void drawRotarySlider(juce::Graphics&, int, int, int, int, float, float, float, juce::Slider&) override;
};


class PluginEditor : public juce::AudioProcessorEditor {
    public:
        explicit PluginEditor(PluginProcessor&);
        ~PluginEditor() override;
        void paint(juce::Graphics&) override;
        void resized() override;

    private:
        PluginProcessor& processorRef;

        HorseshoeLookAndFeel horseshoeLookAndFeel;
        FilterTypeLookAndFeel filterTypeLookAndFeel;

        juce::Slider fcSlider;
        juce::Slider QaSlider;
        juce::Slider QbSlider;
        juce::Slider gSlider;
        juce::Slider volSlider;
        juce::Slider filterTypeSelector;

        juce::SliderParameterAttachment fcAttachment;
        juce::SliderParameterAttachment QaAttachment;
        juce::SliderParameterAttachment QbAttachment;
        juce::SliderParameterAttachment gAttachment;
        juce::SliderParameterAttachment volAttachment;
        juce::SliderParameterAttachment typeAttachment;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};

}