#include "modes.h"
#include "formatters.h"
#include <AceButton.h>

using namespace Modes;

void ModeScale::handleButton(ScaleButton button, uint8_t eventType)
{
    switch (button)
    {
    case BUTTON_TARE:
        switch (eventType)
        {
        case ace_button::AceButton::kEventClicked:
            scale.tare();
            break;
        case ace_button::AceButton::kEventLongPressed:
            scale.calibrate();
            break;
        }
        break;
    }
}

void ModeScale::update(ScaleUpdate update)
{
    if (!update.is_changed_weight)
        return;
    display.fullsize(formatWeight(update.weight));
}

const char *ModeScale::getName()
{
    return "Scale";
}

const char *ModeScale::getID()
{
    return "scale";
}