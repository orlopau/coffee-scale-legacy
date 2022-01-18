#pragma once

#include <HX711_ADC.h>
#include <CircularBuffer.h>
#include "scaleDisplay.h"
#include "constants.h"

struct ScaleUpdate
{
    bool is_new_weight = false;
    bool is_changed_weight = false;
    float weight = 0;
};

struct ScaleMeasurement
{
    unsigned long timestamp;
    float weight;
};

class Scale
{
public:
    Scale(HX711_ADC &hx711, ScaleDisplay &display);
    void setup();
    ScaleUpdate update();
    void calibrate();
    void tare();
    void changeSamples();
    CircularBuffer<ScaleMeasurement, 400> history;
    float lastWeight = 0;

private:
    HX711_ADC &loadCell;
    ScaleDisplay &display;
    int currentAveragingMode = 0;
    // TODO sample sizes greater than 128 dont work at the moment
    const int AVERAGING_MODES[AVERAGING_MODES_SIZE] = {1, 16};
};