#include "scale.h"
#include <EEPROM.h>

Scale::Scale(HX711_ADC &hx711, ScaleDisplay &display) : loadCell(hx711), display(display), stopwatch()
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

void Scale::update()
{
    bool changedWeight = false;
    if (loadCell.update() == 1)
    {
        float weight = loadCell.getData();
        String weightString = String(weight, 2) + "g";
        if (!lastWeightString.equals(weightString))
        {
            changedWeight = true;
            lastWeightString = weightString;
        }
    }

    switch (currentMode)
    {
    case SCALE:
        if (changedWeight)
        {
            display.fullsize(lastWeightString);
        }
        break;
    case SCALE_STOPWATCH:
    {
        String stopwatchText = String((float)stopwatch.getTime() / (float)1000, 1) + "s";
        if (!stopwatchText.equals(lastStopwatchText) || changedWeight)
        {
            display.stopwatch(lastWeightString, stopwatchText);
            lastStopwatchText = stopwatchText;
        }
    }
    break;
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
