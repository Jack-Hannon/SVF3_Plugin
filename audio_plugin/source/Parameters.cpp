// This plugin was developed using Jan Wilczek's (WolfSound)
// Audio Plugin Template:
// https://github.com/JanWilczek/audio-plugin-template


namespace audio_plugin {

namespace {

auto& addParameterToProcessor(juce::AudioProcessor& processor, auto parameter) {
  auto& result = *parameter;
  processor.addParameter(parameter.release());
  return result;
}

// Cutoff frequency parameter
juce::AudioParameterFloat& createFcParameter(
    juce::AudioProcessor& processor) {
  constexpr auto versionHint = 1;
  return addParameterToProcessor(
      processor,
      std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID{"fc", versionHint}, "fc",
          juce::NormalisableRange<float>{60.f, 10000.f, 0.1f, 0.5f}, 10000.f,
          juce::AudioParameterFloatAttributes{}.withLabel("Hz")));
}

// BPF2 resonance parameter
juce::AudioParameterFloat& createQaParameter(
    juce::AudioProcessor& processor) {
  constexpr auto versionHint = 1;
  return addParameterToProcessor(
      processor,
      std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID{"Qa", versionHint}, "Qa",
          juce::NormalisableRange<float>{0.4f, 1.f, 0.01f, 1.f}, 0.4f,
          juce::AudioParameterFloatAttributes{}.withLabel(" ")));
}

// BPF1 resonance parameter
juce::AudioParameterFloat& createQbParameter(
    juce::AudioProcessor& processor) {
  constexpr auto versionHint = 1;
  return addParameterToProcessor(
      processor,
      std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID{"Qb", versionHint}, "Qb",
          juce::NormalisableRange<float>{0.4f, 1.f, 0.01f, 1.f}, 0.4f,
          juce::AudioParameterFloatAttributes{}.withLabel(" ")));
}

// Nonlinearity gain parameter
juce::AudioParameterFloat& createGParameter(
    juce::AudioProcessor& processor) {
  constexpr auto versionHint = 1;
  return addParameterToProcessor(
      processor,
      std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID{"g", versionHint}, "g",
          juce::NormalisableRange<float>{0.0f, 5.5f, 0.001f, 0.7f}, 0.f,
          juce::AudioParameterFloatAttributes{}.withLabel(" ")));
}

// Volume target parameter
juce::AudioParameterFloat& createVolParameter(
    juce::AudioProcessor& processor) {
  constexpr auto versionHint = 1;
  return addParameterToProcessor(
      processor,
      std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID{"vol", versionHint}, "vol",
          juce::NormalisableRange<float>{0.f, 0.1f, 0.0001f, 0.5f}, 0.1f,
          juce::AudioParameterFloatAttributes{}.withLabel(" ")));
}

// Filter type parameter
juce::AudioParameterChoice& createTypeParameter(
    juce::AudioProcessor& processor) {
  constexpr auto versionHint = 1;
  return addParameterToProcessor(
      processor,
      std::make_unique<juce::AudioParameterChoice>(
          juce::ParameterID{"filter_type", versionHint},
          "filter_type", juce::StringArray{"lpf", "bpf1", "bpf2", "hpf"}, 0));
}

}

Parameters::Parameters(juce::AudioProcessor& processor) : fc{createFcParameter(processor)}, Qa{createQaParameter(processor)},
      Qb{createQbParameter(processor)}, g{createGParameter(processor)}, vol{createVolParameter(processor)}, filt_type{createTypeParameter(processor)} {}
}
