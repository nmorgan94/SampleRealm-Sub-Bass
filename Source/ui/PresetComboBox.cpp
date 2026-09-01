#include "PresetComboBox.h"

//==============================================================================
void PresetComboBox::setPresetManager (PresetManager& newPresetManager)
{
    presetManager = &newPresetManager;

    const auto currentName = presetManager->getCurrentPresetName();
    setText (currentName.isNotEmpty() ? currentName : PresetManager::initPresetName, juce::dontSendNotification);
}

void PresetComboBox::promptToSavePreset()
{
    if (presetManager == nullptr)
        return;

    const auto currentName = getText();
    auto* alertWindow = new juce::AlertWindow ("Save Preset",
                                                "Enter a name for this preset:",
                                                juce::MessageBoxIconType::NoIcon,
                                                this);
    alertWindow->addTextEditor ("presetName", currentName == PresetManager::initPresetName ? juce::String() : currentName);
    alertWindow->addButton ("Save", 1, juce::KeyPress (juce::KeyPress::returnKey));
    alertWindow->addButton ("Cancel", 0, juce::KeyPress (juce::KeyPress::escapeKey));

    juce::Component::SafePointer<PresetComboBox> safeThis (this);

    alertWindow->enterModalState (true,
        juce::ModalCallbackFunction::create ([safeThis, alertWindow] (int result)
        {
            if (result != 1 || safeThis == nullptr)
                return;

            const auto name = alertWindow->getTextEditorContents ("presetName").trim();
            if (name.isEmpty())
                return;

            safeThis->applyOutcome (safeThis->presetManager->savePreset (name), name, "Save", "saved");
        }),
        true);
}

void PresetComboBox::promptToDeletePreset()
{
    if (presetManager == nullptr)
        return;

    const auto name = getText().trim();
    if (name.isEmpty() || name == PresetManager::initPresetName)
        return;

    juce::Component::SafePointer<PresetComboBox> safeThis (this);

    juce::AlertWindow::showAsync (
        juce::MessageBoxOptions()
            .withIconType (juce::MessageBoxIconType::WarningIcon)
            .withTitle ("Delete Preset")
            .withMessage ("Delete preset \"" + name + "\"? This can't be undone.")
            .withButton ("Delete")
            .withButton ("Cancel")
            .withAssociatedComponent (this),
        [safeThis, name] (int result)
        {
            if (result != 1 || safeThis == nullptr)
                return;

            safeThis->presetManager->deletePreset (name);
            safeThis->presetManager->resetToDefaults();
            safeThis->setText (PresetManager::initPresetName, juce::dontSendNotification);
        });
}

void PresetComboBox::showPopup()
{
    if (! isEnabled() || presetManager == nullptr)
        return;

    const auto names = presetManager->getAllPresetNames();

    juce::PopupMenu menu;
    for (int i = 0; i < names.size(); ++i)
        menu.addItem (i + 1, names[i]);

    juce::Component::SafePointer<PresetComboBox> safeThis (this);

    menu.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (*this),
        [safeThis, names] (int result)
        {
            if (safeThis == nullptr)
                return;

            safeThis->hidePopup();

            if (result != 0)
            {
                const auto& name = names[result - 1];
                safeThis->applyOutcome (safeThis->presetManager->loadPreset (name), name, "Load", "loaded");
            }
        });
}

void PresetComboBox::applyOutcome (bool succeeded, const juce::String& presetName,
                                   const juce::String& actionTitle, const juce::String& actionPastTense)
{
    if (succeeded)
    {
        setText (presetName, juce::dontSendNotification);
        return;
    }

    juce::AlertWindow::showMessageBoxAsync (juce::MessageBoxIconType::WarningIcon,
        "Couldn't " + actionTitle + " Preset",
        "\"" + presetName + "\" couldn't be " + actionPastTense + ".",
        "OK", this);
}
