#pragma once;

#define EEPROM_ADDR_CALVAL 0
#define KNOWN_MASS 100.0
#define STABILIZING_TIME 2000

#define AVERAGING_MODES_SIZE 2

#define PIN_MODE 14
#define PIN_TARE 12
#define PIN_ONOFF uint8_t(0)

// when this threshold is reached the stopwatch is started
#define LIN_START_THRESHOLD 0.5;
// below this slope the pump is considered stopped
#define LIN_PUMP_STOP_SLOPE 0.3
// the maximum possible time that is displayed
#define LIN_MAX_TIME_TRESHOLD 3 * 60 * 1000
// the weight above which something is calculated
#define LIN_MIN_WEIGHT_THRESHOLD 5

enum ScaleButton
{
    MODE,
    ONOFF,
    TARE,
};

enum ScaleButtonEvent
{
    CLICK,
    CLICK_DOUBLE,
    CLICK_LONG,
};