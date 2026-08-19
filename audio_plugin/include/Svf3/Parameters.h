// This plugin was developed using Jan Wilczek's (WolfSound)
// Audio Plugin Template:
// https://github.com/JanWilczek/audio-plugin-template


#pragma once

namespace audio_plugin {
struct Parameters {
  explicit Parameters(juce::AudioProcessor&);

    juce::AudioParameterFloat& fc;            // cutoff frequency of the filter
    juce::AudioParameterFloat& Qa;            // Resonance parameter - damping for BPF2, Qa = 1/a
    juce::AudioParameterFloat& Qb;            // Resonance parameter - damping for BPF1, Qb = 1/b
    juce::AudioParameterFloat& g;             // Scaling factor for the nonlinear damping function
    juce::AudioParameterFloat& vol;           // Target volume for dynamic gain controller
    juce::AudioParameterChoice& filt_type;    // Active filter output: LPF, BPF1, BPF2, HPF

    JUCE_DECLARE_NON_COPYABLE(Parameters)
    JUCE_DECLARE_NON_MOVEABLE(Parameters)
};
} 
