#ifndef DhtSensor_h
#define DhtSensor_h

#include "DHT.h"

class DhtSensor {
public:
  DhtSensor(u8 pin, u8 type) {
    this->pin = pin;
    this->type = type;
    dht = new DHT(pin, type);
    dht->begin();
  }

  real32 temperature() {
    const real32 t = dht->readTemperature();
    if (isnan(t)) Serial.println((String) "[DhtSensor] Failed to read temperature, pin: " + pin + ", type: " + type);
    return t;
  }

  real32 humidity() {
    const real32 h = dht->readHumidity();
    if (isnan(h)) Serial.println((String) "[DhtSensor] Failed to read humidity, pin: " + pin + ", type: " + type);
    return h;
  }

  String getTelemetry() {

    const real32 t = temperature();
    const real32 h = humidity();

    return String("sensors/dht ") +
           (!isnan(t) ? (String("temperature=") + t + ",") : "") +
           (!isnan(h) ? (String("humidity=")    + h      ) : "");
  }

private:
  DHT *dht;
  u8 pin;
  u8 type;
};

#endif