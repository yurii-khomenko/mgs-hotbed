#include <Gigrostat.h>
#include <Arduino.h>
#include <Fsm.h>

#define IDLE 0
#define INCREASE_HUMIDITY 1
#define DECREACE_HUMIDITY 2

State idle(
[] {
  Serial.println("[Gigrostat] Entering idle");
},
[] {
  //Serial.println("[Gigrostat] In idle");
}, 
[] {
  Serial.println("[Gigrostat] Exiting idle");
});

State increaseHumidity(
[] {
  Serial.println("[Gigrostat] Entering increaseHumidity");
},
[] {
  //Serial.println("[Gigrostat] In increaseHumidity");
}, 
[] {
  Serial.println("[Gigrostat] Exiting increaseHumidity");
});

State decreaseHumidity(
[] {
  Serial.println("[Gigrostat] Entering decreaseHumidity");
},
[] {
 // Serial.println("In decreaseHumidity");
}, 
[] {
  Serial.println("[Gigrostat] Exiting decreaseHumidity");
});

Fsm fsm(&idle);

Gigrostat::Gigrostat(DHT* dht, Humidifier* humidifier, String metricPrefix) {
  this->dht = dht;
  this->humidifier = humidifier;
  this->metricPrefix = metricPrefix;
}

void Gigrostat::setup(const real32 min, const real32 max) {
  
  this->min = min;
  this->max = max;

  fsm.clearTransition();

  fsm.addTransition(&idle, &increaseHumidity, INC_HUMIDITY, [this] () {
    Serial.println("[Gigrostat] Transitioning from idle to increaseHumidity");
    // ventilation->off();
    humidifier->on();
  });
  fsm.addTransition(&increaseHumidity, &idle, IDLE, [this] {
    Serial.println("[Gigrostat] Transitioning from increaseHumidity to idle");
    // ventilation->off();
    humidifier->off();
  });

  fsm.addTransition(&increaseHumidity, &decreaseHumidity, DEC_HUMIDITY, [this] {
    Serial.println("[Gigrostat] Transitioning from increaseHumidity to decreaseHumidity");
    humidifier->off();
    // ventilation->on();
  });
  fsm.addTransition(&decreaseHumidity, &increaseHumidity, INC_HUMIDITY, [this] {
    Serial.println("[Gigrostat] Transitioning from decreaseHumidity to increaseHumidity");
    // ventilation->off();
    humidifier->on();
  });

  fsm.addTransition(&decreaseHumidity, &idle, IDLE, [this] {
    Serial.println("[Gigrostat] Transitioning from decreaseHumidity to idle");
    humidifier->off();
    // ventilation->off();
  });
  fsm.addTransition(&idle, &decreaseHumidity, DEC_HUMIDITY, [this] {
    Serial.println("[Gigrostat] Transitioning from idle to decreaseHumidity");
    humidifier->off();
    // ventilation->on();
  });
}

void Gigrostat::loop() {

  const float current = dht->readHumidity();
  
  if(isnan(current)) {
    Serial.println("[Gigrostat] Failed to read humidity from DHT sensor!");
    return;
  }

  if(current < min)
    fsm.trigger(INC_HUMIDITY);
  else if (current > max)
    fsm.trigger(DEC_HUMIDITY);
  else
    fsm.trigger(IDLE);

  fsm.loop();
}