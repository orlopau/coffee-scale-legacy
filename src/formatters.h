#pragma once
#include <Arduino.h>

inline String formatWeight(float weight)
{
    return String(weight, 2) + "g";
}

inline String formatTime(long time)
{
    return String((float)time / (float)1000, 1) + "s";
}