#include <Arduino.h>
#include <lcdgfx.h>
#include <HX711_ADC.h>
#include <EEPROM.h>
#include <Wire.h>
#include <AceButton.h>
using namespace ace_button;
#include "scaleDisplay.h"
#include "scale.h"
#include "modes.h"

#define PIN_SCL 4
#define PIN_SDA 5
#define PIN_HX711_DATA 13
#define PIN_HX711_CLK 16

#define BAUDRATE 115200

ScaleDisplay display(PIN_SCL, PIN_SDA);
HX711_ADC hx711(PIN_HX711_DATA, PIN_HX711_CLK);
Scale scale(hx711, display);

int currentMode = 0;
Modes::ModeScale *mode_scale = new Modes::ModeScale(display, scale);
Modes::ModeStopwatch *mode_stopwatch = new Modes::ModeStopwatch(display, scale);
Modes::ModeRegression *mode_regression = new Modes::ModeRegression(display, scale);
Modes::ModeSettings *mode_settings = new Modes::ModeSettings(display, scale);

const int MODE_SIZE = 4;
Modes::Mode *modes[] = {
    mode_scale,
    mode_stopwatch,
    mode_regression,
    mode_settings,
};

AceButton btnMode(PIN_MODE);
AceButton btnOnOff(PIN_ONOFF);
AceButton btnTare(PIN_TARE);

ScaleButton aceToButton(AceButton *button)
{
  switch (button->getPin())
  {
  case PIN_MODE:
    return BUTTON_MODE;
  case PIN_ONOFF:
    return BUTTON_ONOFF;
  case PIN_TARE:
    return BUTTON_TARE;
  default:
    Serial.println("ERROR: invalid button conversion");
    return BUTTON_MODE;
  }
};

void changeMode(int mode_index)
{
  modes[currentMode]->leave();
  currentMode = mode_index;

  Modes::Mode *mode = modes[currentMode];
  mode->enter();
  display.fading(mode->getName(), 200);
}

void incrementMode()
{
  if (currentMode + 1 == MODE_SIZE)
  {
    changeMode(0);
  }
  else
  {
    changeMode(currentMode + 1);
  }
}

void handleEvent(AceButton *button, uint8_t eventType, uint8_t buttonState)
{
  ScaleButton scale_button = aceToButton(button);

  switch (scale_button)
  {
  case BUTTON_MODE:
    switch (eventType)
    {
    case AceButton::kEventClicked:
      incrementMode();
      break;
    case AceButton::kEventLongPressed:
      scale.changeSamples();
      break;
    }
    break;
  }

  modes[currentMode]->handleButton(scale_button, eventType);
}

void setup()
{
  Serial.begin(BAUDRATE);
  EEPROM.begin(512);

  Serial.println("setting up");

  // setup display
  Serial.println("init display");
  display.setup();

  // setup buttons
  pinMode(PIN_MODE, INPUT_PULLUP);
  pinMode(PIN_ONOFF, INPUT_PULLUP);
  pinMode(PIN_TARE, INPUT_PULLUP);

  ButtonConfig *buttonConfig = ButtonConfig::getSystemButtonConfig();
  buttonConfig->setEventHandler(handleEvent);
  buttonConfig->setFeature(ButtonConfig::kFeatureClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
  buttonConfig->setClickDelay(400);
  buttonConfig->setDoubleClickDelay(600);
  buttonConfig->setLongPressDelay(1500);

  // setup scale
  scale.setup();
}

void loop()
{
  btnMode.check();
  btnOnOff.check();
  btnTare.check();

  auto scale_update = scale.update();
  modes[currentMode]->update(scale_update);

  display.update();
}
