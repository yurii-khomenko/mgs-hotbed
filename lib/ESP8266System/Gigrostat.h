#ifndef Gigrostat_h
#define Gigrostat_h

#include <Arduino.h>
#include <Fsm.h>
#include <DhtSensor.h>
#include <Humidifier.h>

class Gigrostat {
  public:
    Gigrostat(DhtSensor* dhtSensor, Humidifier* humidifier, String metricPrefix);
    void setup(real32 min, real32 max);
    void loop();
  private:
    DhtSensor* dhtSensor;
    Humidifier* humidifier;
    float min;
    float max;
    String metricPrefix;
};

#endif