#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <array>
#include <functional>

//==============================================================================
/** Background visualization of the ADSR shape, drawn behind the envelope sliders.

    The drive curve is overlaid on the same axes, because the drive envelope is derived
    from this one.
*/
class EnvelopeVisualizer : public juce::Component
{
public:
    EnvelopeVisualizer();

    void setADSR (float attackSeconds, float decaySeconds, float sustainLevel, float releaseSeconds);
    void setDrive (float driveAmount, float envAmount);
    void setCurves (float attackTension, float decayTension, float releaseTension);

    void paint (juce::Graphics&) override;

private:
    struct Breakpoint { float x, env, curve; };

    [[nodiscard]] std::array<Breakpoint, 5> buildBreakpoints (juce::Rectangle<float> bounds) const;
    [[nodiscard]] juce::Path buildPath (juce::Rectangle<float> bounds,
                                        const std::function<float (float)>& levelForEnv) const;

    float attackTime = 0.005f, decayTime = 0.1f, sustainLevel = 0.8f, releaseTime = 0.2f;
    float drive = 0.2f, driveEnvAmount = 0.0f;
    float attackCurve = 0.0f, decayCurve = 0.0f, releaseCurve = 0.0f;
};
