#include "PresetManager.h"

namespace
{
    constexpr const char* presetFileExtension = ".sub";
}

const juce::Identifier PresetManager::currentPresetProperty { "currentPreset" };
const juce::String PresetManager::initPresetName { "Init" };

//==============================================================================
PresetManager::PresetManager (juce::AudioProcessorValueTreeState& stateToUse)
    : apvts (stateToUse)
{
}

juce::File PresetManager::getPresetDirectory()
{
    auto dir = juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
                   .getChildFile ("SampleRealm")
                   .getChildFile ("SubBass")
                   .getChildFile ("Presets");
    dir.createDirectory();
    return dir;
}

juce::Array<juce::File> PresetManager::findPresetFiles()
{
    return getPresetDirectory().findChildFiles (juce::File::findFiles, false, juce::String ("*") + presetFileExtension);
}

juce::File PresetManager::getFileForPreset (const juce::String& name)
{
    const auto sanitized = juce::File::createLegalFileName (name);

    for (const auto& file : findPresetFiles())
        if (file.getFileNameWithoutExtension().equalsIgnoreCase (sanitized))
            return file;

    return getPresetDirectory().getChildFile (sanitized + presetFileExtension);
}

juce::StringArray PresetManager::getAllPresetNames() const
{
    juce::StringArray names;

    for (const auto& file : findPresetFiles())
        names.add (file.getFileNameWithoutExtension());

    names.sort (true);
    names.insert (0, initPresetName);
    return names;
}

juce::String PresetManager::getCurrentPresetName() const
{
    return apvts.state.getProperty (currentPresetProperty, "").toString();
}

void PresetManager::resetToDefaults()
{
    for (auto* param : apvts.processor.getParameters())
        param->setValueNotifyingHost (param->getDefaultValue());

    apvts.state.setProperty (currentPresetProperty, initPresetName, nullptr);
}

bool PresetManager::savePreset (const juce::String& name)
{
    const auto sanitized = juce::File::createLegalFileName (name.trim());
    if (sanitized.isEmpty() || sanitized == initPresetName)
        return false;

    auto stateToSave = apvts.copyState();
    stateToSave.removeProperty (currentPresetProperty, nullptr);

    const auto xml = stateToSave.createXml();
    if (xml == nullptr || ! xml->writeTo (getFileForPreset (sanitized)))
        return false;

    apvts.state.setProperty (currentPresetProperty, sanitized, nullptr);
    return true;
}

bool PresetManager::loadPreset (const juce::String& name)
{
    if (name == initPresetName)
    {
        resetToDefaults();
        return true;
    }

    const auto file = getFileForPreset (name);
    if (! file.existsAsFile())
        return false;

    const auto xml = juce::XmlDocument::parse (file);
    if (xml == nullptr)
        return false;

    apvts.replaceState (juce::ValueTree::fromXml (*xml));
    apvts.state.setProperty (currentPresetProperty, name, nullptr);
    return true;
}

bool PresetManager::deletePreset (const juce::String& name)
{
    if (name == initPresetName)
        return false;

    const auto file = getFileForPreset (name);
    return file.existsAsFile() && file.deleteFile();
}
