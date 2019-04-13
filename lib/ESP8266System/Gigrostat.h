#ifndef Gigrostat_h
#define Gigrostat_h

#include <DhtSensor.h>
#include <Humidifier.h>
#include <Ventilation.h>

class Gigrostat {
  public:
    Gigrostat(DhtSensor* dhtSensor, Humidifier* humidifier, Ventilation* ventilation, String metricPrefix);
    void setup(real32 min, real32 max);
    void loop();
  private:
    DhtSensor* dhtSensor;
    Humidifier* humidifier;
    Ventilation* ventilation;
    String metricPrefix;
    real32 min;
    real32 max;
};

#endif