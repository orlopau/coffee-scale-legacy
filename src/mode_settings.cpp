#include "modes.h"
#include "storage.h"
#include <AceButton.h>

using namespace Modes;

void ModeSettings::enter()
{
    storage::updateData();
    initialDraw = false;
    hasChanges = false;
}

void ModeSettings::leave()
{
    if (hasChanges)
        storage::saveData();
}

void ModeSettings::handleButton(ScaleButton button, uint8_t eventType)
{
    switch (button)
    {
    case BUTTON_ONOFF:
        switch (eventType)
        {
        case ace_button::AceButton::kEventClicked:
            changeCurrentSettingValue(DECREMENT);
            break;
        case ace_button::AceButton::kEventLongPressed:
            incrementSetting();
            break;
        }
        break;
    case BUTTON_TARE:
        switch (eventType)
        {
        case ace_button::AceButton::kEventClicked:
            changeCurrentSettingValue(INCREMENT);
            break;
        }
    }
}

void ModeSettings::update(ScaleUpdate update)
{
    // update when fading is over
    if (!initialDraw && !display.isFading())
    {
        updateDisplay();
        initialDraw = true;
    }
}

const char *ModeSettings::getName()
{
    return "Settings";
}

const char *ModeSettings::getID()
{
    return "settings";
}

void ModeSettings::updateDisplay()
{
    auto setting = storage::data.entries[currentSetting];
    display.setting(setting.name, String(setting.value, 2));
}

void ModeSettings::incrementSetting()
{
    if (currentSetting + 1 >= SETTINGS_SIZE)
    {
        currentSetting = 0;
    }
    else
    {
        currentSetting++;
    }

    updateDisplay();
}

void ModeSettings::changeCurrentSettingValue(Delta type)
{
    hasChanges = true;
    if (type == INCREMENT)
    {
        storage::data.entries[currentSetting].value += storage::data.entries[currentSetting].increment;
    }
    else
    {
        storage::data.entries[currentSetting].value -= storage::data.entries[currentSetting].increment;
    }
    updateDisplay();
}