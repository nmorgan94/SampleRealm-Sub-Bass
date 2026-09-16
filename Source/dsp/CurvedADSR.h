#pragma once

#include <juce_core/juce_core.h>
#include "Curve.h"

//==============================================================================
/**
    An ADSR envelope whose attack, decay and release can each be bent by a curve.
*/
class CurvedADSR
{
public:
    struct Parameters
    {
        float attack = 0.1f, decay = 0.1f, sustain = 1.0f, release = 0.1f;
        float attackCurve = 0.0f, decayCurve = 0.0f, releaseCurve = 0.0f;
    };

    void setSampleRate (double newSampleRate) noexcept
    {
        sampleRate = static_cast<float> (newSampleRate);
    }

    void setParameters (const Parameters& newParameters) noexcept
    {
        parameters = newParameters;

        const auto isRamping = stage == Stage::attack || stage == Stage::decay || stage == Stage::release;
        const auto shape = shapeFor (stage);

        // Redrawing the stage with a changed curve or target would jump the level onto the new line,
        // so carry on from the current level over the time that was left instead.
        if (isRamping && (! juce::exactlyEqual (shape.curve, stageCurve) || ! juce::exactlyEqual (shape.target, stageEndLevel)))
            beginRamp (timeScale * (1.0f - progress));
    }

    [[nodiscard]] bool isActive() const noexcept { return stage != Stage::idle; }

    void reset() noexcept
    {
        stage = Stage::idle;
        level = 0.0f;
    }

    void noteOn() noexcept
    {
        startStage (Stage::attack);
    }

    void noteOff() noexcept
    {
        if (stage != Stage::idle)
            startStage (Stage::release);
    }

    float getNextSample() noexcept
    {
        switch (stage)
        {
            case Stage::idle:
                return 0.0f;

            case Stage::sustain:
            {
                level = parameters.sustain;
                break;
            }

            case Stage::attack:
            case Stage::decay:
            case Stage::release:
            {
                const auto finished = advance (shapeFor (stage).seconds * timeScale);
                level = stageStartLevel + (stageEndLevel - stageStartLevel) * shapeProgress (progress, stageCurve);

                if (finished)
                    finishStage();

                break;
            }
        }

        return level;
    }

private:
    enum class Stage { idle, attack, decay, sustain, release };

    struct StageShape { float seconds, target, curve; };

    // How long the given stage takes, the level it heads to, and how it bends, from the current parameters.
    [[nodiscard]] StageShape shapeFor (Stage rampStage) const noexcept
    {
        switch (rampStage)
        {
            case Stage::attack:  return { parameters.attack, 1.0f, parameters.attackCurve };
            case Stage::decay:   return { parameters.decay, parameters.sustain, parameters.decayCurve };
            case Stage::release: return { parameters.release, 0.0f, parameters.releaseCurve };
            case Stage::idle:
            case Stage::sustain: break;
        }

        return { 0.0f, 0.0f, 0.0f };
    }

    void startStage (Stage newStage) noexcept
    {
        stage = newStage;

        // An attack starting part-way up only climbs the rest of the way, which keeps juce::ADSR's rate.
        beginRamp (stage == Stage::attack ? 1.0f - level : 1.0f);
    }

    void beginRamp (float newTimeScale) noexcept
    {
        const auto shape = shapeFor (stage);
        stageStartLevel = level;
        stageEndLevel = shape.target;
        stageCurve = shape.curve;
        timeScale = newTimeScale;
        progress = 0.0f;
    }

    void finishStage() noexcept
    {
        if (stage == Stage::attack)
            startStage (Stage::decay);
        else if (stage == Stage::decay)
            startStage (Stage::sustain);
        else
            reset();
    }

    bool advance (float stageSeconds) noexcept
    {
        progress = stageSeconds > 0.0f ? std::min (1.0f, progress + 1.0f / (stageSeconds * sampleRate))
                                       : 1.0f;
        return progress >= 1.0f;
    }

    Parameters parameters;
    float sampleRate = 44100.0f;

    Stage stage = Stage::idle;
    float level = 0.0f;
    float stageStartLevel = 0.0f;
    float stageEndLevel = 0.0f;
    float stageCurve = 0.0f;
    float timeScale = 1.0f;
    float progress = 0.0f;
};
