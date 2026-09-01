#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../PresetManager.h"

//==============================================================================
/**
    A ComboBox that drives its own PopupMenu instead of ComboBox's built-in item list.
*/
class PresetComboBox : public juce::ComboBox
{
public:
    void setPresetManager (PresetManager& newPresetManager);
    void promptToSavePreset();
    void promptToDeletePreset();

private:
    void showPopup() override;

    void applyOutcome (bool succeeded, const juce::String& presetName,
                       const juce::String& actionTitle, const juce::String& actionPastTense);

    PresetManager* presetManager = nullptr;
};
