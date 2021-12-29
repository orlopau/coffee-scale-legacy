#pragma once

#include "stopwatch.h"
#include <HX711_ADC.h>
#include "scaleDisplay.h"
#include <AceButton.h>
using namespace ace_button;

#define EEPROM_ADDR_CALVAL 0
#define KNOWN_MASS 100.0
#define STABILIZING_TIME 2000

#define AVERAGING_MODES_SIZE 2

#define PIN_MODE 14
#define PIN_TARE 12
#define PIN_ONOFF uint8_t(0)

// when this threshold is reached the stopwatch is started
#define LIN_START_THRESHOLD = 0.5;
// the timeframe over which the linear regression is run
#define LIN_WINDOW_TIME = 5000;
// below this slope the pump is considered stopped
#define LIN_PUMP_STOP_SLOPE = 0.3

enum Mode
{
    SCALE,
    SCALE_STOPWATCH,
    SCALE_REGRESSION
};
const int MODE_SIZE = 3;
const String MODE_NAMES[MODE_SIZE] = {"Scale", "Timer", "Calc"};

struct CalcState
{
    long estimatedTime;
    float slope;
    float offset;
    float measurements[];
};

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
    Mode currentMode = SCALE;
    String lastWeightString;
    int currentAveragingMode = 0;
    // TODO sample sizes greater than 128 dont work at the moment
    const int AVERAGING_MODES[AVERAGING_MODES_SIZE] = {1, 16};
    String lastStopwatchText;
    CalcState calcState = {};
};