#ifndef Ventilation_h
#define Ventilation_h

#include <Arduino.h>
#include <ArduinoJson.h>

class Ventilation {
public:
  Ventilation(u8 pin);

  String metrics();
  void setState(const DynamicJsonDocument &state);

  real32 getFlow() { return flow; }
  void setFlow(real32 levelPercent);

private:
  u8 pin;
  real32 flow = 0;
};

#endif