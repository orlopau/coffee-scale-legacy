#pragma once

#include "constants.h"
#include "scaleDisplay.h"
#include "scale.h"
#include <Stopwatch.h>
#include <Regression.h>

namespace Modes
{
    class Mode
    {
    public:
        Mode(ScaleDisplay &display, Scale &scale) : display(display), scale(scale){};
        virtual ~Mode(){};
        virtual void enter() {}
        virtual void leave() {}
        virtual void update(ScaleUpdate update) {}
        virtual void handleButton(ScaleButton button, uint8_t eventType) = 0;
        virtual const char *getName() = 0;

    protected:
        ScaleDisplay &display;
        Scale &scale;
    };

    class ModeScale : public Mode
    {
    public:
        ModeScale(ScaleDisplay &display, Scale &scale) : Mode(display, scale){};
        virtual void update(ScaleUpdate update);
        virtual void handleButton(ScaleButton button, uint8_t eventType);
        virtual const char *getName();
    };

    class ModeStopwatch : public Mode
    {
    public:
        ModeStopwatch(ScaleDisplay &display, Scale &scale) : Mode(display, scale){};
        virtual void update(ScaleUpdate update);
        virtual void handleButton(ScaleButton button, uint8_t eventType);
        virtual const char *getName();

    private:
        String lastTimeText;
        Stopwatch stopwatch;
    };

    class ModeRegression : public Mode
    {
    public:
        ModeRegression(ScaleDisplay &display, Scale &scale) : Mode(display, scale){};
        virtual void update(ScaleUpdate update);
        virtual void handleButton(ScaleButton button, uint8_t eventType);
        virtual const char *getName();

    private:
        String lastTimeText;
        Stopwatch stopwatch;
        regression::Approximation<50> approximation;
    };

    class ModeSettings : public Mode
    {
    public:
        ModeSettings(ScaleDisplay &display, Scale &scale) : Mode(display, scale){};
        virtual void enter();
        virtual void leave();
        virtual void update(ScaleUpdate update);
        virtual void handleButton(ScaleButton button, uint8_t eventType);
        virtual const char *getName();

    private:
        enum Delta
        {
            INCREMENT,
            DECREMENT,
        };
        int currentSetting = 0;
        bool initialDraw = false;
        bool hasChanges = false;
        void updateDisplay();
        void incrementSetting();
        void changeCurrentSettingValue(Delta type);
    };
}