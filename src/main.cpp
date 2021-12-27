#include <Arduino.h>
#include <lcdgfx.h>
#include <HX711_ADC.h>
#include <EEPROM.h>
#include <Wire.h>
#include <AceButton.h>
using namespace ace_button;
#include "scaleDisplay.h"
#include "scale.h"

#define PIN_SCL 4
#define PIN_SDA 5
#define PIN_HX711_DATA 13
#define PIN_HX711_CLK 16

#define BAUDRATE 115200

ScaleDisplay display(PIN_SCL, PIN_SDA);
HX711_ADC hx711(PIN_HX711_DATA, PIN_HX711_CLK);
Scale scale(hx711, display);

AceButton btnMode(PIN_MODE);
AceButton btnOnOff(PIN_ONOFF);
AceButton btnTare(PIN_TARE);

void handleEvent(AceButton *button, uint8_t eventType, uint8_t buttonState) {
  scale.handleEvent(button, eventType, buttonState);
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

String lastWeightString;

void loop()
{
  btnMode.check();
  btnOnOff.check();
  btnTare.check();

  scale.update();

  display.update();
}
