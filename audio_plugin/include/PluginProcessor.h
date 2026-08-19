// This plugin was developed using Jan Wilczek's (WolfSound)
// Audio Plugin Template:
// https://github.com/JanWilczek/audio-plugin-template

#pragma once

#include "Svf3/Parameters.h"
#include "Svf3/Svf3.h"

namespace audio_plugin {

class PluginProcessor : public juce::AudioProcessor {

  public:
    PluginProcessor();

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    [[nodiscard]] Parameters& getParameterRefs() noexcept;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

  private:

    audio_plugin::Parameters parameters{*this};
    audio_plugin::Svf3 svf3;
    std::atomic<double> currentSampleRate{0.};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};

}
