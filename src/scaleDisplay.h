#pragma once

#include <lcdgfx.h>

class ScaleDisplay
{
    public:
        ScaleDisplay(int8_t scl, int8_t sda);
        void update();
        void setup();
        void info(String line1, String line2 = "");
        void fullsize(String line, bool force = false);
        void fading(String text, int ms);
        void stopwatch(String weight, String time);
        void regression(String weight, String time, String regression);
        void setting(String name, String value);
        bool isFading();
    private:
        DisplaySSD1306_128x32_I2C display;
        unsigned long fadeTimeout;
};