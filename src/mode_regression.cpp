#include "modes.h"
#include "formatters.h"
#include <AceButton.h>
#include "storage.h"

using namespace Modes;

void ModeRegression::handleButton(ScaleButton button, uint8_t eventType)
{
    switch (button)
    {
    case BUTTON_ONOFF:
        switch (eventType)
        {
        case ace_button::AceButton::kEventClicked:

        case ace_button::AceButton::kEventLongPressed:
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

void ModeRegression::update(ScaleUpdate update)
{
    if (!update.is_new_weight)
        return;

    // only do something when the stopwatch is running
    String regressionText = "Waiting";
    if (stopwatch.isRunning() && update.weight > LIN_MIN_WEIGHT_THRESHOLD)
    {
        approximation.addPoint({stopwatch.getTime(), update.weight});
        float targetWeight = storage::data.entries[storage::SETTING_REG_TARGET_WEIGHT].value;
        auto result = approximation.getLeastSquares();
        result.yIntercept += storage::data.entries[storage::SETTING_REG_OFFSET].value;
        lastEstimatedTime = approximation.getXAtY(targetWeight, result);
        long timeLeft = lastEstimatedTime - stopwatch.getTime();
        Serial.println("time left: " + String(timeLeft));

        if (update.weight >= targetWeight)
        {
            regressionText = "Done";
            stopwatch.stop();
        }
        else if (timeLeft < LIN_MAX_TIME_TRESHOLD && timeLeft > 0)
        {
            Serial.println("target: " + String(lastEstimatedTime));
            regressionText = formatTime(-timeLeft);
        }
    }
    else
    {
        approximation.reset();
    }

    display.regression(
        formatWeight(update.weight),
        formatTime(stopwatch.getTime()),
        regressionText);
}

const char *ModeRegression::getName()
{
    return "Calc";
}

const char *ModeRegression::getID()
{
    return "regression";
}

void ModeRegression::createJSONSummary(JsonObject &object)
{
    object["targetWeight"] = storage::data.entries[storage::SETTING_REG_TARGET_WEIGHT].value;
    object["stopwatchStart"] = stopwatch.getStartTime();
    object["estimatedTime"] = lastEstimatedTime + stopwatch.getStartTime();
}