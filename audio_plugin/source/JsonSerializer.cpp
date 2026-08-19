// This plugin was developed using Jan Wilczek's (WolfSound)
// Audio Plugin Template:
// https://github.com/JanWilczek/audio-plugin-template

namespace audio_plugin {

    namespace {

        struct SerializableParameters {

            float fc;
            float Qa;
            float Qb;
            float g;
            float vol;
            int filt_type;

            static constexpr auto marshallingVersion = 1;

            template <typename Archive, typename T>
            static void serialise(Archive& archive, T& t)
            {
                using namespace juce;

                if (archive.getVersion() != 1)
                    return;

                std::string pluginName = JUCE_PLUGIN_NAME;

                archive(
                    named("pluginName", pluginName),
                    named("fc", t.fc),
                    named("Qa", t.Qa),
                    named("Qb", t.Qb),
                    named("g", t.g),
                    named("vol", t.vol),
                    named("filt_type", t.filt_type)
                );
            }
        };
    
        SerializableParameters from(const Parameters& parameters)
        {
            return {
                .fc = parameters.fc.get(),
                .Qa = parameters.Qa.get(),
                .Qb = parameters.Qb.get(),
                .g = parameters.g.get(),
                .vol = parameters.vol.get(),
                .filt_type = parameters.filt_type.getIndex()
            };
        }
    }

    void JsonSerializer::serialize(const Parameters& parameters, juce::OutputStream& output)
    {
        const auto parametersToSerialize = from(parameters);
        const auto json = juce::ToVar::convert(parametersToSerialize);

        if (!json.has_value())
            return;

        juce::JSON::writeToStream(output, *json, juce::JSON::FormatOptions{}.withSpacing(juce::JSON::Spacing::multiLine).withMaxDecimalPlaces(2));
    }

    juce::Result JsonSerializer::deserialize(juce::InputStream& input, Parameters& parameters)
    {
        juce::var parsedResult;

        const auto result = juce::JSON::parse(input.readEntireStreamAsString(), parsedResult);

        if (result.failed())
            return result;

        const auto parsedParameters = juce::FromVar::convert<SerializableParameters>(parsedResult);

        if (!parsedParameters.has_value())
            return juce::Result::fail("failed to parse parameters from JSON representation");

        parameters.filt_type = parsedParameters->filt_type;
        parameters.fc = parsedParameters->fc;
        parameters.Qa = parsedParameters->Qa;
        parameters.Qb = parsedParameters->Qb;
        parameters.g = parsedParameters->g;
        parameters.vol = parsedParameters->vol;

        return juce::Result::ok();
    }
}