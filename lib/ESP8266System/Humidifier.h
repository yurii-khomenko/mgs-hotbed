#ifndef Humidifier_h
#define Humidifier_h

#include <Arduino.h>

class Humidifier {
  public:
    Humidifier(u8 pin, String metricPrefix);
    void on();
    void off();
    String getMetrics();
    std::vector<String> getMetricsList();
  private:
    u8 pin;
    bool enable;
    String metricPrefix;
};

#endif