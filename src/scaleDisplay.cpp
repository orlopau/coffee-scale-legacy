#include "scaleDisplay.h"

ScaleDisplay::ScaleDisplay(int8_t scl, int8_t sda)
    : display(uint32_t(-1), {int8_t(-1), uint8_t(0x3C),
                             int8_t(scl), int8_t(sda), uint32_t(-1)}) {}

void ScaleDisplay::setup()
{
    display.setFixedFont(ssd1306xled_font6x8);
    display.begin();
    display.getInterface().flipHorizontal(1);
    display.getInterface().flipVertical(1);
    display.clear();
}

void ScaleDisplay::update()
{
}

bool ScaleDisplay::isFading()
{
    return millis() <= fadeTimeout;
}

void ScaleDisplay::info(String line1, String line2)
{
    if (!isFading())
    {
        display.setFixedFont(ssd1306xled_font6x8);
        display.clear();
        display.printFixed(0, 0, line1.c_str());
        display.printFixed(0, 16, line2.c_str());
    }
}

void ScaleDisplay::fullsize(String line, bool force)
{
    if (isFading() && !force)
    {
        return;
    }

    display.setFixedFont(ssd1306xled_font8x16);
    display.clear();
    display.printFixedN(0, 0, line.c_str(), STYLE_NORMAL, 1);
}

void ScaleDisplay::fading(String text, uint ms)
{
    fadeTimeout = millis() + ms;
    fullsize(text, true);
}

void ScaleDisplay::stopwatch(String weight, String time)
{
    if (isFading())
    {
        return;
    }

    display.clear();
    display.setFixedFont(ssd1306xled_font6x8);
    display.printFixed(0, 18, time.c_str());
    display.printFixedN(0, 0, weight.c_str(), STYLE_NORMAL, 1);
}