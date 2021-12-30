#include "scale.h"
#include <EEPROM.h>
#include <climits>

Scale::Scale(HX711_ADC &hx711, ScaleDisplay &display) : loadCell(hx711), display(display), stopwatch(), approximation()
{
}

void Scale::setup()
{
    display.info("Coffeescale", "Version 1.0");

    loadCell.begin();
    loadCell.setSamplesInUse(AVERAGING_MODES[currentAveragingMode]);

    float calVal;
    EEPROM.get(EEPROM_ADDR_CALVAL, calVal);
    if (!isnan(calVal))
    {
        loadCell.setCalFactor(calVal);
    }

    loadCell.start(2000, true);
    if (loadCell.getTareTimeoutFlag() || loadCell.getSignalTimeoutFlag())
    {
        display.info("Load cell error");
        while (1)
            ;
    }

    while (!loadCell.update())
        ;
}

static String formatWeight(float weight)
{
    return String(weight, 2) + "g";
}

static String formatTime(long time)
{
    return String((float)time / (float)1000, 1) + "s";
}

void Scale::update()
{
    bool changedWeight = false;
    float weight = NAN;
    if (loadCell.update() == 1)
    {
        weight = round(loadCell.getData() * 100) / 100;
        if (lastWeight != weight)
        {
            changedWeight = true;
            lastWeight = weight;
        }
    }

    switch (currentMode)
    {
    case SCALE:
        if (changedWeight)
        {
            display.fullsize(formatWeight(lastWeight));
        }
        break;
    case SCALE_STOPWATCH:
    {
        String stopwatchText = formatTime(stopwatch.getTime());
        if (!stopwatchText.equals(lastStopwatchText) || changedWeight)
        {
            display.stopwatch(formatWeight(lastWeight), stopwatchText);
            lastStopwatchText = stopwatchText;
        }
        break;
    }
    case SCALE_REGRESSION:
    {
        if (isnan(weight))
            break;

        // only do something when the stopwatch is running
        String regressionText = "Waiting";
        if (stopwatch.isRunning() && weight > LIN_MIN_WEIGHT_THRESHOLD)
        {
            approximation.addPoint({stopwatch.getTime(), weight});
            long targetTime = approximation.getXAtY(targetWeight);
            long timeLeft = targetTime - stopwatch.getTime();
            Serial.println("time left: " + String(timeLeft));

            if (weight >= targetWeight || timeLeft < 0)
            {
                regressionText = "Done";
            }
            else if (timeLeft < LIN_MAX_TIME_TRESHOLD && timeLeft > 0)
            {
                Serial.println("target: " + String(targetTime));
                regressionText = formatTime(-timeLeft);
            }
        }
        else
        {
            Serial.println("points reset");
            approximation.reset();
        }

        display.regression(
            formatWeight(lastWeight),
            formatTime(stopwatch.getTime()),
            regressionText);
        break;
    }
    default:
        display.info("invalid mode", String(currentMode));
        break;
    }
}

void Scale::calibrate()
{
    display.info("starting calibration");

    loadCell.update();
    loadCell.tare();
    display.info("tare complete");
    delay(1000);
    display.info("add weight", String(KNOWN_MASS));
    delay(5000);

    loadCell.refreshDataSet();
    float newCalibrationValue = loadCell.getNewCalibration(KNOWN_MASS);

    display.info("new calibration value", String(newCalibrationValue, 4));

    EEPROM.put(EEPROM_ADDR_CALVAL, newCalibrationValue);
    EEPROM.commit();
    EEPROM.get(EEPROM_ADDR_CALVAL, newCalibrationValue);

    Serial.print("value ");
    Serial.print(newCalibrationValue);
    Serial.print(" saved to EEPROM address: ");
    Serial.println(EEPROM_ADDR_CALVAL);

    display.info("calibration finished with", String(newCalibrationValue, 2));
    delay(1000);
}

void Scale::tare()
{
    display.fullsize("Tare...");
    loadCell.tareNoDelay();
}

void Scale::changeMode(Mode mode)
{
    currentMode = mode;
    display.fading(MODE_NAMES[mode], 200);
}

void Scale::incrementMode()
{
    stopwatch.reset();
    approximation.reset();

    if (currentMode + 1 == MODE_SIZE)
    {
        changeMode((Mode)0);
    }
    else
    {
        changeMode((Mode)(currentMode + 1));
    }
}

void Scale::changeSamples()
{
    if (currentAveragingMode + 1 >= AVERAGING_MODES_SIZE)
    {
        currentAveragingMode = 0;
    }
    else
    {
        currentAveragingMode++;
    }

    int samples = AVERAGING_MODES[currentAveragingMode];
    loadCell.setSamplesInUse(samples);
    Serial.print("samples in use: ");
    Serial.println(samples);
    display.fading("SS: " + String(samples), 500);
}

void Scale::handleEvent(AceButton *button, uint8_t eventType, uint8_t buttonState)
{
    switch (button->getPin())
    {
    case PIN_MODE:
        switch (eventType)
        {
        case AceButton::kEventClicked:
            incrementMode();
            break;
        case AceButton::kEventLongPressed:
            changeSamples();
            break;
        }
        break;
    case PIN_ONOFF:
        switch (eventType)
        {
        case AceButton::kEventClicked:
            switch (currentMode)
            {
            case SCALE_STOPWATCH:
            case SCALE_REGRESSION:
                stopwatch.toggle();
                break;
            }
            break;
        }
        break;
    case PIN_TARE:
        switch (eventType)
        {
        case AceButton::kEventClicked:
            tare();
            break;
        case AceButton::kEventDoubleClicked:
            calibrate();
            break;
        }
        break;
    }
}
