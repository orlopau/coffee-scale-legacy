#pragma once

class Stopwatch
{
public:
    void stop();
    void start();
    void toggle();
    void reset();
    bool isRunning();
    long getTime();
    long getStartTime();

private:
    long lastStartTime = 0;
    long lastStoppedTime = 0;
};