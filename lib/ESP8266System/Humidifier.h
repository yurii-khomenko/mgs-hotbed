#ifndef Humidifier_h
#define Humidifier_h

#include <Arduino.h>

class Humidifier {
  public:
    Humidifier(u8 pin, u8 statePin, String metricPrefix);
    void on();
    void off();
    bool isOn();
    String getMetrics();
    std::vector<String> getMetricsList();
  private:
    u8 pin;
    u8 statePin;
    String metricPrefix;
    void click();
};

#endif