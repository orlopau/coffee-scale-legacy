#pragma once

class Stopwatch
{
public:
    void stop();
    void start();
    void toggle();
    bool isRunning();
    long getTime();

private:
    long lastStartTime = 0;
    long lastStoppedTime = 0;
};