
namespace audio_plugin {

void HorseshoeLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) {
    
    const float radius = juce::jmin(width, height) * 0.38f;
    const float centreX = x + width * 0.5f;
    const float centreY = y + height * 0.5f;
    const float lineWidth = 8.0f;
    const float trackStartAngle = rotaryStartAngle + juce::MathConstants<float>::halfPi;
    const float trackEndAngle = rotaryEndAngle + juce::MathConstants<float>::halfPi;

    juce::Path track;
    track.addCentredArc(centreX, centreY, radius, radius, 0.0f, trackStartAngle, trackEndAngle, true);

    g.setColour(juce::Colours::grey);
    g.strokePath(track, juce::PathStrokeType(lineWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    const float indicatorStart = radius * 0.48f;
    const float indicatorLength = radius;
    const float x1 = centreX + std::cos(angle) * indicatorStart;
    const float y1 = centreY + std::sin(angle) * indicatorStart;
    const float x2 = centreX + std::cos(angle) * indicatorLength;
    const float y2 = centreY + std::sin(angle) * indicatorLength;

    g.setColour(juce::Colours::black);
    g.drawLine(x1, y1, x2, y2, 5.0f);
}

void FilterTypeLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider&)
{
    const float radius = juce::jmin(width, height) * 0.38f;
    const float centreX = x + width * 0.5f;
    const float centreY = y + height * 0.5f;
    const float trackWidth = 8.0f;
    const float startAngle = rotaryStartAngle;
    const float endAngle = rotaryEndAngle;
    const float trackStartAngle = startAngle + juce::MathConstants<float>::halfPi;
    const float trackEndAngle = endAngle + juce::MathConstants<float>::halfPi;

    juce::Path track;
    track.addCentredArc(centreX, centreY, radius, radius, 0.0f, trackStartAngle, trackEndAngle, true);

    g.setColour(juce::Colours::grey);
    g.strokePath(track, juce::PathStrokeType(trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    const char* labels[] = { "LP", "BP1", "BP2", "HP " };

    for (int i = 0; i < 4; ++i)
    {
        const float proportion = static_cast<float>(i) / 3.0f;
        const float angle = startAngle + proportion * (endAngle - startAngle);
        const float nodeX = centreX + std::cos(angle) * radius;
        const float nodeY = centreY + std::sin(angle) * radius;

        g.setColour(juce::Colours::black);
        g.fillEllipse(nodeX - 4.0f, nodeY - 4.0f, 8.0f, 8.0f);

        const float labelRadius = radius + 14.0f;
        const float labelX = centreX + std::cos(angle) * labelRadius;
        const float labelY = centreY + std::sin(angle) * labelRadius;

        g.setFont(juce::Font(11.f));
        g.drawText(labels[i], static_cast<int>(labelX - 24.0f), static_cast<int>(labelY - 8.0f), 48, 16, juce::Justification::centred, false);
    }

    const float selectedAngle = startAngle + sliderPosProportional * (endAngle - startAngle);
    const float indicatorStart = radius * 0.48f;
    const float indicatorLength = radius;
    const float x1 = centreX + std::cos(selectedAngle) * indicatorStart;
    const float y1 = centreY + std::sin(selectedAngle) * indicatorStart;
    const float x2 = centreX + std::cos(selectedAngle) * indicatorLength;
    const float y2 = centreY + std::sin(selectedAngle) * indicatorLength;

    g.setColour(juce::Colours::black);
    g.drawLine(x1, y1, x2, y2, 5.0f);
}

PluginEditor::PluginEditor(PluginProcessor& p)
    : AudioProcessorEditor(&p),
      processorRef(p),
      fcAttachment(p.getParameterRefs().fc, fcSlider),
      QaAttachment(p.getParameterRefs().Qa, QaSlider),
      QbAttachment(p.getParameterRefs().Qb, QbSlider),
      gAttachment(p.getParameterRefs().g, gSlider),
      volAttachment(p.getParameterRefs().vol, volSlider),
      typeAttachment(p.getParameterRefs().filt_type, filterTypeSelector)
{
    setSize(400, 320);

    const float startAngle = juce::MathConstants<float>::pi * 0.8f;
    const float endAngle = juce::MathConstants<float>::pi * 2.2f;

    for (auto* slider : { &fcSlider, &QaSlider, &QbSlider, &gSlider, &volSlider })
    {
        slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider->setRotaryParameters(startAngle, endAngle, true);
        slider->setLookAndFeel(&horseshoeLookAndFeel);
        addAndMakeVisible(slider);
    }

    filterTypeSelector.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    filterTypeSelector.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    filterTypeSelector.setRotaryParameters(startAngle, endAngle, true);
    filterTypeSelector.setLookAndFeel(&filterTypeLookAndFeel);
    addAndMakeVisible(filterTypeSelector);
}

PluginEditor::~PluginEditor()
{
    fcSlider.setLookAndFeel(nullptr);
    QaSlider.setLookAndFeel(nullptr);
    QbSlider.setLookAndFeel(nullptr);
    gSlider.setLookAndFeel(nullptr);
    volSlider.setLookAndFeel(nullptr);
    filterTypeSelector.setLookAndFeel(nullptr);
}

void PluginEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(210, 204, 242));

    g.setColour(juce::Colours::black);
    g.setFont(juce::Font(18.0f, juce::Font::bold));
    g.drawText("3RD ORDER NONLINEAR FILTER", 15, 15, getWidth() - 30, 30, juce::Justification::centred, false);

    g.setFont(juce::Font(14.0f, juce::Font::bold));

    g.drawText("fc", 40, 135, 80, 20, juce::Justification::centred, false);
    g.drawText("Qa", 150, 135, 80, 20, juce::Justification::centred, false);
    g.drawText("Qb", 260, 135, 80, 20, juce::Justification::centred, false);

    g.drawText("Nonlinearity", 40, 245, 80, 20, juce::Justification::centred, false);
    g.drawText("Gain", 40, 260, 80, 20, juce::Justification::centred, false);

    g.drawText("Volume", 150, 245, 80, 20, juce::Justification::centred, false);
    g.drawText("Limiter", 150, 260, 80, 20, juce::Justification::centred, false);

    g.drawText("Filter", 260, 245, 80, 20, juce::Justification::centred, false);
    g.drawText("Type", 260, 260, 80, 20, juce::Justification::centred, false);

    g.setColour(juce::Colours::black);
    g.setFont(juce::Font(12.0f));
    g.drawText("Jack Hannon 2026", getWidth() - 105, getHeight() - 18, 100, 12, juce::Justification::right, false);
}

void PluginEditor::resized()
{
    fcSlider.setBounds(40, 70, 80, 80);
    QaSlider.setBounds(150, 70, 80, 80);
    QbSlider.setBounds(260, 75, 80, 80);
    gSlider.setBounds(40, 180, 80, 80);
    volSlider.setBounds(150, 180, 80, 80);
    filterTypeSelector.setBounds(260, 180, 80, 80);
}
}