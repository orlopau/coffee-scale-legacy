#pragma once

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

enum Mode
{
    SCALE,
    SCALE_STOPWATCH
};
const int MODE_SIZE = 2;
const String MODE_NAMES[MODE_SIZE] = {"Scale", "Timer"};

struct StopwatchState
{
    long lastStartTime;
    long lastStoppedTime;
    String lastText;
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
    void toggleStopwatch();
    void handleEvent(AceButton *button, uint8_t eventType, uint8_t buttonState);

private:
    Mode currentMode = SCALE;
    HX711_ADC &loadCell;
    ScaleDisplay &display;
    String lastWeightString;
    int currentAveragingMode = 1;
    // TODO sample sizes greater than 128 dont work at the moment
    const int AVERAGING_MODES[AVERAGING_MODES_SIZE] = {1, 16};
    StopwatchState stopwatchState = {0, 0};
};