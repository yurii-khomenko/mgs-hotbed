#include <DhtSensor.h>
#include <DHT.h>

DhtSensor::DhtSensor(u8 pin, u8 type, String metricPrefix) {
  this->metricPrefix = metricPrefix;
  dht = new DHT(pin, type);
  dht->begin();
}

real32 DhtSensor::getTemperature() {
  return dht->readTemperature();
}

real32 DhtSensor::getHumidity() {
  return dht->readHumidity();
}

String DhtSensor::getMetrics() {
  return String(metricPrefix + "temperature ") + getTemperature() + "\n";
}