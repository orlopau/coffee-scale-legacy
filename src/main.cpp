#include <Arduino.h>
#include <WiFiManager.h>
#include <lcdgfx.h>
#include <HX711_ADC.h>
#include <EEPROM.h>
#include <Wire.h>
#include <AceButton.h>
using namespace ace_button;
#include "scaleDisplay.h"
#include "scale.h"
#include "modes.h"
#include "storage.h"

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

WiFiManager wifi_manager;
WiFiManagerParameter param_ip("ip", "IP", "", 40);
WiFiManagerParameter param_gateway("gateway", "Gateway", "", 40);
WiFiManagerParameter param_subnet_mask("subnet", "Subnet mask", "255.255.255.0", 40);

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

void paramsCallback()
{
  IPAddress ip;
  IPAddress gateway;
  IPAddress subnet_mask;

  Serial.println("saved params");

  bool success = ip.fromString(param_ip.getValue()) && gateway.fromString(param_gateway.getValue()) && subnet_mask.fromString(param_subnet_mask.getValue());
  if (!success)
  {
    Serial.println(String("invalid ip settings: ") + String(param_ip.getValue()) + String(param_gateway.getValue()) + String(param_subnet_mask.getValue()));
    wifi_manager.resetSettings();
    wifi_manager.reboot();
  }

  wifi_manager.setSTAStaticIPConfig(ip, gateway, subnet_mask);
  storage::updateData();
  storage::data.network = {ip, gateway, subnet_mask};
  storage::saveData();
}

void setup()
{
  Serial.begin(BAUDRATE);
  EEPROM.begin(512);
  WiFi.mode(WIFI_STA);
  storage::updateData();

  Serial.println("setting up");

  // setup buttons
  pinMode(PIN_MODE, INPUT_PULLUP);
  pinMode(PIN_ONOFF, INPUT_PULLUP);
  pinMode(PIN_TARE, INPUT_PULLUP);

  // reset when buttons pressed
  if (!digitalRead(PIN_MODE) && !digitalRead(PIN_TARE))
  {
    wifi_manager.resetSettings();
  }

  // setup wifi
  auto network = storage::data.network;
  wifi_manager.setSTAStaticIPConfig(network.ip, network.gateway, network.subnet);
  wifi_manager.addParameter(&param_ip);
  wifi_manager.addParameter(&param_gateway);
  wifi_manager.addParameter(&param_subnet_mask);
  wifi_manager.setConfigPortalBlocking(false);
  wifi_manager.setClass("invert");
  wifi_manager.setDebugOutput(false);
  wifi_manager.setSaveParamsCallback(paramsCallback);
  if (wifi_manager.autoConnect("Coffee Scale"))
  {
    Serial.println("connected to wifi");
  }
  else
  {
    Serial.println("not connected to wifi, running captive portal");
  }

  // setup display
  Serial.println("init display");
  display.setup();

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

  wifi_manager.process();
}
