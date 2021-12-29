#include "stopwatch.h"
#include <Arduino.h>

void Stopwatch::stop()
{
    lastStoppedTime = getTime();
    lastStartTime = 0;
}

void Stopwatch::start()
{
    lastStartTime = millis();
}

void Stopwatch::toggle()
{
    if (isRunning())
    {
        stop();
    }
    else
    {
        start();
    }
}

bool Stopwatch::isRunning()
{
    return lastStartTime != 0;
}

long Stopwatch::getTime()
{
    if (isRunning())
    {
        return millis() - lastStartTime;
    }
    else
    {
        return lastStoppedTime;
    }
}
