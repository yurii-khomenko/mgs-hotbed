#include <Gigrostat.h>
#include <Arduino.h>

Gigrostat::Gigrostat(DHT *dht, Humidifier *humidifier, String metricPrefix) {
  this->dht = dht;
  this->humidifier = humidifier;
  this->metricPrefix = metricPrefix;
}

void Gigrostat::setup(const float humidity, const float accuracy) {
  this->humidity = humidity;
  this->accuracy = accuracy;
}

void Gigrostat::loop() {
  
  const float current = dht->readHumidity();
  
  if(isnan(current)) {
    Serial.println("[Gigrostat] Failed to read humidity from DHT sensor!");
    return;
  }
  
  const float min = humidity - accuracy; 
  const float max = humidity + accuracy;

  if(current < min) {
    humidifier->on();
    Serial.println("[Gigrostat] humidifier->on()");
  } else if (current > max) {
    humidifier->off();
    Serial.println("[Gigrostat] humidifier->off()");
  } else {
    Serial.println("[Gigrostat] humidity ok, min: " + String(min) + ", max: " + String(max));
  }
}