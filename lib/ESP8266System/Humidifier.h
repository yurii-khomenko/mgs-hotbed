#ifndef Humidifier_h
#define Humidifier_h

#include <Arduino.h>

class Humidifier {
  public:
    Humidifier(u8 pin);
    void on();
    void off();
    void getMetrics();
  private:
    u8 pin;
    bool enable;
};

#endif