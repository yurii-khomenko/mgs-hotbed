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
  
  const float actualHumidity = dht->readHumidity();
  
  if(isnan(actualHumidity)) {
    Serial.println("[Gigrostat] Failed to read humidity from DHT sensor!");
    return;
  }
  
  const float min = humidity - accuracy; 
  const float max = humidity + accuracy;

  // if()
  //   sys.offPin(HUMIDIFIER);
  // else
  //   sys.onPin(HUMIDIFIER);

  Serial.println("[Gigrostat] Hold method is invoked, min: " + String(min) + ", max: " + String(max));
}