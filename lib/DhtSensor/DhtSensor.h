#ifndef DhtSensor_h
#define DhtSensor_h

#include <DHT.h>

class DhtSensor {
  public:
    DhtSensor(u8 pin, u8 type, String metricPrefix);
    real32 getTemperature();
    real32 getHumidity();
    String getMetrics();
  private:
    DHT* dht;
    u8 pin;
    u8 type;
    String metricPrefix;
};

#endif