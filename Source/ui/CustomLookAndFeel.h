#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    static inline const juce::Colour background { 0xff0b0d11 };
    static inline const juce::Colour panel { 0xff14171d };
    static inline const juce::Colour panelBorder { 0xff262b33 };
    static inline const juce::Colour accent { 0xff2fe8c4 };
    static inline const juce::Colour text { 0xffd8dee6 };
    static inline const juce::Colour textDim { 0xff7a828e };

    CustomLookAndFeel();
    ~CustomLookAndFeel() override = default;

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                            float sliderPosProportional, float rotaryStartAngle,
                            float rotaryEndAngle, juce::Slider&) override;

    juce::Font getLabelFont (juce::Label&) override;

    [[nodiscard]] static juce::FontOptions orbitronRegular();
    [[nodiscard]] static juce::FontOptions orbitronBold();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomLookAndFeel)
};
