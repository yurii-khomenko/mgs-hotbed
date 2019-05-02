#ifndef Humidifier_h
#define Humidifier_h

#include <Arduino.h>
#include <ArduinoJson.h>

class Humidifier {
public:
  Humidifier(u8 pin, u8 statePin) {
    this->pin = pin;
    this->statePin = statePin;
    pinMode(pin, OUTPUT_OPEN_DRAIN);
    pinMode(statePin, INPUT);
  }

  u8 getFlow() {
    return (u8) digitalRead(statePin) ? 100 : 0;
  }

  void setFlow(u8 level) {
    if (level <= 0 && getFlow() >= 100)   click();
    else if (level > 0 && getFlow() <= 0) click();
  }

  void setSpec(const DynamicJsonDocument &spec) {
    setFlow(spec["actuators"]["humidifier"]["flow"] | getFlow());
  }

  String getStatus() {
    return String("actuators/humidifier flow=") + getFlow();
  }

private:
  u8 pin;
  u8 statePin;

  void click() {
    digitalWrite(pin, LOW);
    delay(200);
    digitalWrite(pin, HIGH);
  }
};

#endif