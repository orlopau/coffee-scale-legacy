#include "scale.h"
#include <climits>
#include "storage.h"

Scale::Scale(HX711_ADC &hx711, ScaleDisplay &display) : loadCell(hx711), display(display)
{
}

void Scale::setup()
{
    display.info("Coffeescale", "Version 1.0");

    loadCell.begin();
    loadCell.setSamplesInUse(AVERAGING_MODES[currentAveragingMode]);

    storage::updateData();
    if (!isnan(storage::data.cal_val) && storage::data.cal_val != 0)
    {
        loadCell.setCalFactor(storage::data.cal_val);
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

ScaleUpdate Scale::update()
{
    bool changed_weight = false;
    bool new_weight = false;
    if (loadCell.update() == 1)
    {
        const float weight = round(loadCell.getData() * 100) / 100;
        new_weight = true;
        if (lastWeight != weight)
        {
            changed_weight = true;
            lastWeight = weight;
        }
    }

    return {new_weight, changed_weight, lastWeight};
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

    storage::data.cal_val = newCalibrationValue;
    storage::saveData();

    Serial.print("value ");
    Serial.print(newCalibrationValue);
    Serial.println(" saved to EEPROM");

    display.info("calibration finished with", String(newCalibrationValue, 2));
    delay(1000);
}

void Scale::tare()
{
    display.fullsize("Tare...");
    loadCell.tareNoDelay();
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