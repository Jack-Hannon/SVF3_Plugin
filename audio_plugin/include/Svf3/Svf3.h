// This plugin was developed using Jan Wilczek's (WolfSound)
// Audio Plugin Template:
// https://github.com/JanWilczek/audio-plugin-template

#pragma once

#include <iostream>
#include <algorithm>
#include <cmath>

// The minimum bandpass feedback gain controls how close the complex pole pair may get to the imaginary axis
// The minimum value is 1 (marginally stable oscillator with Re{complex poles} = 0)
// Set > 1 for safety
#define MIN_BP_FDBK_GAIN 1.012f

namespace audio_plugin {

class Svf3 {
    public:

        Svf3() { }

        enum class FilterType : size_t {lpf = 0, bpf1 = 1, bpf2 = 2, hpf = 3};
        static constexpr std::array<FilterType, 4> types = {FilterType::lpf, FilterType::bpf1, FilterType::bpf2, FilterType::hpf};

        void prepare(double sampleRate, int expectedMaxFramesPerBlock, int numChannels) {
            juce::ignoreUnused(expectedMaxFramesPerBlock);
            FS = sampleRate;

            // Initialize useful constants for IIR coefficient computations
            chnn_filt_states.resize(numChannels);
            FSp3m8  = 8.0 * std::pow(sampleRate, 3);
            FSp3m24 = 24.0 * std::pow(sampleRate, 3);
            FSp2m4  = 4.0 * std::pow(sampleRate, 2);
            FSm2    = 2.0 * sampleRate;     
        }

        // Update BPF2 Resonance Parameter
        void setQa(float Qa_input) {
            Qa = 0.95 * Qa + 0.05 * Qa_input;
        }

        // Update BPF1 Resonance Parameter
        void setQb(float Qb_input) {
            Qb = 0.95 * Qb + 0.05 * Qb_input;
        }

        // Update gain for feedback nonlinearity
        void setG(float g_input) {
            g = 0.95 * g + 0.05 * g_input;
        }
        
        // Update cutoff frequency
        void setFc(float fc_input) {
            fc_input = std::clamp(static_cast<double>(fc_input), 1.0, 0.49 * FS);
            fc = 0.95 * fc + 0.05 * fc_input;
        }

        // Update target volume for dynamic gain controller
        void setVol(float vol_input) {
            vol = 0.95 * vol + 0.05 * vol_input;
        }

        // Update active filter output
        void setFilterType(FilterType filt_type) {
            filter_type = filt_type;
        }

        void process(juce::AudioBuffer<float>& buffer) noexcept {

            // Update warped cutoff frequency
            const double wd = (juce::MathConstants<double>::pi * fc)/FS;
            const double wcd = 2 * FS * std::tan(wd);

            // Compute inverted resonance factors (gains for BP feedback)
            const float alpha = MIN_BP_FDBK_GAIN / Qa;
            const float beta = MIN_BP_FDBK_GAIN / Qb;

            // Precompute commonly used freq terms
            const double wcd2 = wcd * wcd;
            const double wcd3 = wcd2 * wcd;

            // Iterate through frames
            for (int frame_idx = 0; frame_idx < buffer.getNumSamples(); frame_idx++) {

                // Iterate through channels of the current frame
                for (int chnn_idx = 0; chnn_idx < buffer.getNumChannels(); chnn_idx++) {

                    // Acquire a reference to the target channel's FilterState struct holding the past IIR states, LPF output, and envelope follower values
                    FilterState& state = chnn_filt_states[chnn_idx];
                    const auto input = buffer.getSample(chnn_idx, frame_idx);
                    state.inputEnvelopeVal = 0.9999 * state.inputEnvelopeVal + 0.0001 * std::fabs(input);

                    // Perform three successive approximations of the current LPF output
                    uint8_t num_estimations = 3;
                    for (uint8_t i = 0; i < num_estimations; i++) {
                        float nl_lpf_fdbk_gain = wcd3 * (1 - NLfold(state.past_lpf));
                        compute_a_coeffs(alpha, beta, wcd, wcd2, nl_lpf_fdbk_gain);
                        state.past_lpf = compute_lpf_estimation(wcd3, input, state);
                    }

                    float b0, b1, b2, b3;

                    // Compute 'b' coefficients for the IIR filter based on the target filter output
                    switch (filter_type) {
                        case FilterType::bpf1:
                            b0 = static_cast<float>(FSm2 * beta * wcd2 / a0d);
                            b1 = b0;
                            b2 = -b0;
                            b3 = b2;
                            break;
                        case FilterType::bpf2:
                            b0 = static_cast<float>(FSp2m4 * alpha * wcd / a0d);
                            b1 = -b0;
                            b2 = b1;
                            b3 = b0;
                            break;
                        case FilterType::hpf:
                            b0 = static_cast<float>(FSp3m8 / a0d);
                            b1 = -3.f * b0;
                            b2 = -b1;
                            b3 = -b0;
                            break;
                    }

                    // Compute filter output
                    state.z0 = input + (state.z1 * a1) + (state.z2 * a2) + (state.z3 * a3);
                    float out = (filter_type == FilterType::lpf) ? state.past_lpf : (state.z0 * b0 + state.z1 * b1 + state.z2 * b2 + state.z3 * b3);
                    
                    // Shift IIR states (IIR unit delay chain)
                    state.z3 = state.z2;
                    state.z2 = state.z1;
                    state.z1 = state.z0;

                    // Preliminary normalization of the output signal by the nonlinearity gain
                    out = out / (1.f + g);
                    
                    // Update the output envelope follower
                    state.outputEnvelopeVal = (std::fabs(out) > 0.003) ? (0.9999 * state.outputEnvelopeVal + 0.0001 * std::fabs(out)) : state.outputEnvelopeVal;

                    // Scale the output signal so that it matches the input amplitude if the ouput amplitude is less than the input amplitude
                    // *** Focus on boosting signals that have been excessively attenuated
                    float envRatio = state.inputEnvelopeVal / (state.outputEnvelopeVal + 0.001f);
                    envRatio = std::clamp(envRatio, 1.0f, 3.0f);
                    out *= envRatio;

                    // Define scaling factor to match target amplitude/volume
                    // *** Focus on attenuating signals that have begun self oscillating with an amplitude above the volume threshold
                    float targetGain = vol / (state.outputEnvelopeVal * envRatio + 0.001f);
                    targetGain = std::clamp(targetGain, 0.0f, 1.25f);
                    out *= targetGain;

                    // Define a saturation ceiling based on the output envelope value
                    // *** Focus on clipping transient signals that spike in amplitude
                    float satCeil = std::sqrt(state.outputEnvelopeVal);
                    satCeil = std::clamp(satCeil, 0.05f, 0.385f);
                    out = output_saturation(out, satCeil);

                    buffer.setSample(chnn_idx, frame_idx, out);
                }         
            }
        }
            
        void reset() noexcept {

            // Initialize the FilterState structs for each audio channel
            for (auto& state : chnn_filt_states)
                state = FilterState{}; 
        }


    private:

        struct FilterState {
            float z0 = 0.0, z1 = 0.0, z2 = 0.0, z3 = 0.0;
            float past_lpf = 0.0;
            float inputEnvelopeVal = 0.05f, outputEnvelopeVal = 0.05f;
        };

        // Soft clipping function for output safety
        float output_saturation(float in, float asymptote = 1.f) {
            float abs_in = (in >= 0.f) ? in : -in;
            return (asymptote * in / (0.5f + abs_in));
        }

        // Nonlinear LPF feedback gain function
        float NLfold(float in) {
             return (g / (1 + in*in));
        }


        void compute_a_coeffs(float alpha, float beta, double wcd, double wcd2, float nl_lpf_fdbk_gain) {
            
            // Compute raw 'a' coefficients
            double bp1_term = FSm2 * beta * wcd2;
            double bp2_term = FSp2m4 * alpha * wcd;

            a0d = FSp3m8 + bp2_term + bp1_term + nl_lpf_fdbk_gain;
            double a1d = FSp3m24 + bp2_term - bp1_term - 3*nl_lpf_fdbk_gain;
            double a2d = -FSp3m24 + bp2_term + bp1_term - 3*nl_lpf_fdbk_gain;
            double a3d = FSp3m8 - bp2_term + bp1_term - nl_lpf_fdbk_gain;

            // compute normalized 'a' coefficients (force a0 = 1)
            a1 = static_cast<float>(a1d / a0d);
            a2 = static_cast<float>(a2d / a0d);
            a3 = static_cast<float>(a3d / a0d);
        }

        float compute_lpf_estimation(double wcd3, float input, FilterState& state) {

            // Compute 'b' coefficients for LPF case (required no matter the selected filter output to compute the nonlinear feedback term)
            float b0_lpf = static_cast<float>(wcd3 / a0d);
            float b1_lpf = 3.f * b0_lpf;
            float b2_lpf = b1_lpf;
            float b3_lpf = b0_lpf;

            float z0 = input + (state.z1 * a1) + (state.z2 * a2) + (state.z3 * a3);
            return (z0 * b0_lpf + state.z1 * b1_lpf + state.z2 * b2_lpf + state.z3 * b3_lpf);
        }

        // initialize to default of 48kHz
        double FS = 48000;
        double FSp3m8 = 884736000000000;      // 8*fs^3
        double FSp3m24 = 2654208000000000;    // 24*fs^3
        double FSp2m4 = 9216000000;           // 4*fs^2
        double FSm2 = 96000;                  // 2*fs

        // Store IIR coeffs as member variables to simplify recursive LPF estimation
        double a0d;
        float a1, a2, a3;

        float Qa = 0.5f, Qb = 0.5f;                     // raw resonance parameters
        float g = 0.0f;                                 // nonlinearity scaling factor
        float vol = 1.f;                                // target volume value
        float fc = 10000.0f;                            // tunable cutoff frequency (Hz)
        
        std::vector<FilterState> chnn_filt_states;
        FilterType filter_type = FilterType::lpf; 
};
};


