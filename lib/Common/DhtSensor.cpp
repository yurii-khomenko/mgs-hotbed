#include <DhtSensor.h>
#include <DHT.h>

DhtSensor::DhtSensor(u8 pin, u8 type, String metricPrefix) {
  this->pin = pin;
  this->type = type;
  this->metricPrefix = metricPrefix;
  dht = new DHT(pin, type);
  dht->begin();
}

real32 DhtSensor::temperature() {
  const real32 t = dht->readTemperature();
  if (isnan(t)) Serial.println((String) "[DhtSensor] Failed to read temperature, pin: " + pin + ", type: " + type);
  return t;
}

real32 DhtSensor::humidity() {
  const real32 h = dht->readHumidity();
  if (isnan(h)) Serial.println((String) "[DhtSensor] Failed to read humidity, pin: " + pin + ", type: " + type);
  return h;
}

String DhtSensor::metrics() {

  const real32 t = temperature();
  const real32 h = humidity();

  return String("") +
         (!isnan(t) ? (metricPrefix + "temperature_level " + t + "\n") : "") +
         (!isnan(h) ? (metricPrefix + "humidity_level " + h + "\n") : "");
}