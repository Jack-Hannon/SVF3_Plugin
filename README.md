# Nonlinear 3rd Order State Variable Filter

**Jack Hannon, 2026**

This repository contains JUCE C++ code for a nonlinear multimode audio filter. The project was inspired by Peter Blasser's analog "Sprott" Filter, a third-order jerk filter related to the chaotic systems studied by J. C. Sprott.

This project explores the transition from linear to chaotic behavior in a third-order state variable filter. The resulting multimode audio filter provides four distinct filter outputs (lowpass, highpass, and two asymmetric bandpass outputs), two resonance parameters, cutoff frequency control, and a nonlinearity gain control.

<img width="495" height="393" alt="image" src="https://github.com/user-attachments/assets/d52261a1-13ba-49db-b752-13e48aa93739" />

The system behaves as a traditional resonant filter with the nonlinearity gain set to zero. In this state, the (Q_a) and (Q_b) resonance parameters control the feedback gain of the bandpass outputs. Since the bandpass outputs have asymmetric frequency responses, detuning the (Q_a) and (Q_b) resonance parameters can be used to emphasize lower or higher frequencies. The (Q_a) parameter controls the feedback gain of BPFa (BPF2), which has a roll-off of ~12 dB/octave below the cutoff frequency and ~6 dB/octave above the cutoff frequency. The (Q_b) parameter controls the feedback gain of BPFb (BPF1), which has a mirrored frequency response compared to BPFa.

When the nonlinearity gain surpasses 1, the system undergoes a bifurcation (supercritical pitchfork) that changes the stability of the system. When (Q_a) and (Q_b) are sufficiently high, the system undergoes a second bifurcation (subcritical Hopf), and the filter transforms into a forced, nonlinear oscillator. This bifurcation sequence is similar to the route to chaos observed in the Lorenz system, though the underlying dynamics are dissimilar. A butterfly/double-scroll attractor may be visualized by plotting the LPF and BPFb outputs against each other.

An output gain control with dynamic limiting is included to control the output volume and prevent excessive clipping. The volume limiter can be used to set the target output level.

## Notes / Warnings

* **This filter is highly resonant.** Test the waters without headphones when using the filter with maximum resonance. I have observed spikes of approximately 12 dB when scanning the cutoff frequency over harmonics of the input signal.

* **Chaos is not always fun to listen to.** To explore the nonlinear behavior, I recommend patching a simple sawtooth oscillator to the input, setting the resonance parameters close to their maximum values (without headphones at first), tuning the cutoff frequency close to one of the first few harmonics, and then slowly increasing the nonlinearity gain. You should be able to hear the system progressing through the first and second bifurcations. I have found the most interesting behavior near the bifurcation boundaries.

Feel free to email me at [jackhannon777@gmail.com](mailto:jackhannon777@gmail.com).

**Acknowledgment:** This project was developed using Jan Wilczek's [WolfSound Audio Plugin Template](https://github.com/JanWilczek/audio-plugin-template).
