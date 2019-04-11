#ifndef Gigrostat_h
#define Gigrostat_h

#include <Arduino.h>
#include <Fsm.h>
#include <DHT.h>
#include <Humidifier.h>

class Gigrostat {
  public:
    Gigrostat(DHT *dht, Humidifier *humidifier, String metricPrefix);
    void setup(float humidity, float accuracy);
    void loop();
  private:
    DHT *dht;
    Humidifier *humidifier;
    float humidity;
    float accuracy;
    String metricPrefix;
};

#endif