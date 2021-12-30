#pragma once

#include "Stopwatch.h"
#include <HX711_ADC.h>
#include "scaleDisplay.h"
#include <Regression.h>
#include <AceButton.h>
#include "constants.h"
using namespace ace_button;

enum Mode
{
    SCALE,
    SCALE_STOPWATCH,
    SCALE_REGRESSION
};
const int MODE_SIZE = 3;
const String MODE_NAMES[MODE_SIZE] = {"Scale", "Timer", "Calc"};

class Scale
{
public:
    Scale(HX711_ADC &hx711, ScaleDisplay &display);
    void setup();
    void update();
    void calibrate();
    void tare();
    void changeMode(Mode mode);
    void incrementMode();
    void changeSamples();
    void handleEvent(AceButton *button, uint8_t eventType, uint8_t buttonState);

private:
    HX711_ADC &loadCell;
    ScaleDisplay &display;
    Stopwatch stopwatch;
    // use 50 samples, because sample rate is 10 samples/s
    regression::Approximation<50> approximation;
    float targetWeight = 37;
    Mode currentMode = SCALE;
    // last weight rounded to 2 decimal places
    float lastWeight;
    int currentAveragingMode = 0;
    // TODO sample sizes greater than 128 dont work at the moment
    const int AVERAGING_MODES[AVERAGING_MODES_SIZE] = {1, 16};
    String lastStopwatchText;
};