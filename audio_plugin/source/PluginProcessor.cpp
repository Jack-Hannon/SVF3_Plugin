// This plugin was developed using Jan Wilczek's (WolfSound)
// Audio Plugin Template:
// https://github.com/JanWilczek/audio-plugin-template

#include "audio_plugin.h"

namespace audio_plugin {
PluginProcessor::PluginProcessor()
    : AudioProcessor(
          BusesProperties()
#if !JUCE_IS_MIDI_EFFECT
#if !JUCE_IS_SYNTH
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      ) {
}

const juce::String PluginProcessor::getName() const {
  return JUCE_PLUGIN_NAME;
}

bool PluginProcessor::acceptsMidi() const {
#if JUCE_NEEDS_MIDI_INPUT
  return true;
#else
  return false;
#endif
}

bool PluginProcessor::producesMidi() const {
#if JUCE_NEEDS_MIDI_OUTPUT
  return true;
#else
  return false;
#endif
}

bool PluginProcessor::isMidiEffect() const {
#if JUCE_IS_MIDI_EFFECT
  return true;
#else
  return false;
#endif
}

double PluginProcessor::getTailLengthSeconds() const {
  return 0.0;
}

int PluginProcessor::getNumPrograms() {
  return 1;  // NB: some hosts don't cope very well if you tell them there are 0
             // programs, so this should be at least 1, even if you're not
             // really implementing programs.
}

int PluginProcessor::getCurrentProgram() {
  return 0;
}

void PluginProcessor::setCurrentProgram(int index) {
  juce::ignoreUnused(index);
}

const juce::String PluginProcessor::getProgramName(int index) {
  juce::ignoreUnused(index);
  return {};
}


void PluginProcessor::changeProgramName(int index, const juce::String& newName) {
  juce::ignoreUnused(index, newName);
}


void PluginProcessor::prepareToPlay(double sampleRate, int expectedMaxFramesPerBlock) {
  currentSampleRate = sampleRate;
  svf3.prepare(sampleRate, expectedMaxFramesPerBlock, juce::jmax(getTotalNumInputChannels(), getTotalNumOutputChannels()));
}

void PluginProcessor::releaseResources() {
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
  svf3.reset();

}

bool PluginProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
#if JUCE_IS_MIDI_EFFECT
  juce::ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  // Some plugin hosts, such as certain GarageBand versions, will only
  // load plugins that support stereo bus layouts.
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;

  // This checks if the input layout matches the output layout
#if !JUCE_IS_SYNTH
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}

void PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                   juce::MidiBuffer& midiMessages) {
  juce::ignoreUnused(midiMessages);

  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  // In case we have more outputs than inputs, this code clears any output
  // channels that didn't contain input data, (because these aren't
  // guaranteed to be empty - they may contain garbage).
  // This is here to avoid people getting screaming feedback
  // when they first compile a plugin, but obviously you don't need to keep
  // this code if your algorithm always overwrites all the output channels.
  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
    buffer.clear(i, 0, buffer.getNumSamples());
  }
  svf3.setFc(parameters.fc);
  svf3.setG(parameters.g);
  svf3.setQa(parameters.Qa);
  svf3.setQb(parameters.Qb);
  svf3.setVol(parameters.vol);
  
  const auto filterType = static_cast<Svf3::FilterType>(parameters.filt_type.getIndex());
  svf3.setFilterType(filterType);
  svf3.process(buffer);  
}

bool PluginProcessor::hasEditor() const {
  return true;  // (change this to false if you choose to not supply an editor)
}

Parameters& PluginProcessor::getParameterRefs() noexcept {
  return parameters;
}

juce::AudioProcessorEditor* PluginProcessor::createEditor() {
  return new PluginEditor(*this);
}

void PluginProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    DBG("========== GET STATE CALLED ==========");

    juce::MemoryOutputStream outputStream{destData, false};
    JsonSerializer::serialize(parameters, outputStream);

    DBG("STATE SIZE: " + juce::String((int) destData.getSize()));

    DBG("STATE DATA:");
    DBG(juce::String::fromUTF8(
        static_cast<const char*>(destData.getData()),
        static_cast<int>(destData.getSize())));
}

void PluginProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    DBG("========== SET STATE CALLED ==========");
    DBG("STATE SIZE: " + juce::String(sizeInBytes));

    DBG("STATE DATA:");
    DBG(juce::String::fromUTF8(
        static_cast<const char*>(data),
        sizeInBytes));

    juce::MemoryInputStream inputStream{
        data,
        static_cast<size_t>(sizeInBytes),
        false
    };

    const auto result =
        JsonSerializer::deserialize(inputStream, parameters);

    if (result.failed())
        DBG("STATE LOAD FAILED: " + result.getErrorMessage());
    else
        DBG("STATE LOAD SUCCESS");
}
}  // namespace audio_plugin

// This creates new instances of the plugin.
// This function definition must be in the global namespace.
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new audio_plugin::PluginProcessor();
}
