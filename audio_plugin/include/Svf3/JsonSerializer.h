// This plugin was developed using Jan Wilczek's (WolfSound)
// Audio Plugin Template:
// https://github.com/JanWilczek/audio-plugin-template


namespace audio_plugin {
    class JsonSerializer {
        public:
            static void serialize(const Parameters&, juce::OutputStream&);
            static juce::Result deserialize(juce::InputStream&, Parameters&);
    };
}
