#ifndef Humidifier_h
#define Humidifier_h

#include <Arduino.h>
#include <ArduinoJson.h>

class Humidifier {
public:
  Humidifier(u8 pin, u8 statePin);

  String metrics();
  void setState(const DynamicJsonDocument &state);

  u8 getFlow();
  void setFlow(u8 level);

private:
  u8 pin;
  u8 statePin;

  void click();
};

#endif