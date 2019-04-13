#ifndef Ventilation_h
#define Ventilation_h

#include <Arduino.h>

class Ventilation {
  public:
    Ventilation(u8 pin, String metricPrefix);
    u8 level;
    void setup(u8 level);
    String metrics();
  private:
    u8 pin;
    String metricPrefix;
};

#endif