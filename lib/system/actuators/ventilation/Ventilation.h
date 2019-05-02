#ifndef Ventilation_h
#define Ventilation_h

#include <Arduino.h>
#include <ArduinoJson.h>

class Ventilation {
public:
  Ventilation(u8 pin) {
    this->pin = pin;
    pinMode(pin, OUTPUT);
  }

  real32 getFlow() { return flow; }

  void setFlow(real32 levelPercent) {

    if      (levelPercent == flow)  return;
    else if (levelPercent <= 0)     flow = 0;
    else if (levelPercent >= 100)   flow = 100;
    else                            flow = levelPercent;

    if (flow <= 0)                  digitalWrite(pin, LOW);
    else                            digitalWrite(pin, HIGH);
  }

  void setConfig(const DynamicJsonDocument &config) {
    setFlow(config["actuators"]["ventilation"]["flow"] | flow);
  }

  String getTelemetry() {
    return String("actuators/ventilation flow=") + flow;
  }

private:
  u8 pin;
  real32 flow = 0;
};

#endif