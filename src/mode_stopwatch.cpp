#include "modes.h"
#include "formatters.h"
#include <AceButton.h>

using namespace Modes;

void ModeStopwatch::handleButton(ScaleButton button, uint8_t eventType)
{
    switch (button)
    {
    case BUTTON_ONOFF:
        switch (eventType)
        {
        case ace_button::AceButton::kEventClicked:
            stopwatch.toggle();
            break;
        }
        break;
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

void ModeStopwatch::update(ScaleUpdate update)
{
    String timeText = formatTime(stopwatch.getTime());
    if (!lastTimeText.equals(timeText) || update.is_changed_weight)
    {
        display.stopwatch(formatWeight(update.weight), timeText);
        lastTimeText = timeText;
    }
}

const char *ModeStopwatch::getName()
{
    return "Timer";
}

const char *ModeStopwatch::getID()
{
    return "stopwatch";
}
