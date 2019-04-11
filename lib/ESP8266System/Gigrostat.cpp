#include <Gigrostat.h>
#include <Arduino.h>
#include <Fsm.h>

#define IDLE 0
#define INCREASE 1
#define DECREASE 2

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
  Serial.println("[Gigrostat] Entering increase");
},
[] {
  //Serial.println("[Gigrostat] In increase");
}, 
[] {
  Serial.println("[Gigrostat] Exiting increase");
});

State decreaseHumidity(
[] {
  Serial.println("[Gigrostat] Entering decrease");
},
[] {
 // Serial.println("In decrease");
}, 
[] {
  Serial.println("[Gigrostat] Exiting decrease");
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

  fsm.addTransition(&idle, &increaseHumidity, INCREASE, [this] () {
    Serial.println("[Gigrostat] Transitioning from idle to increase");
    // ventilation->off();
    humidifier->on();
  });
  fsm.addTransition(&increaseHumidity, &idle, IDLE, [this] {
    Serial.println("[Gigrostat] Transitioning from increase to idle");
    // ventilation->off();
    humidifier->off();
  });

  fsm.addTransition(&increaseHumidity, &decreaseHumidity, DECREASE, [this] {
    Serial.println("[Gigrostat] Transitioning from increase to decrease");
    humidifier->off();
    // ventilation->on();
  });
  fsm.addTransition(&decreaseHumidity, &increaseHumidity, INCREASE, [this] {
    Serial.println("[Gigrostat] Transitioning from decrease to increase");
    // ventilation->off();
    humidifier->on();
  });

  fsm.addTransition(&decreaseHumidity, &idle, IDLE, [this] {
    Serial.println("[Gigrostat] Transitioning from decrease to idle");
    humidifier->off();
    // ventilation->off();
  });
  fsm.addTransition(&idle, &decreaseHumidity, DECREASE, [this] {
    Serial.println("[Gigrostat] Transitioning from idle to decrease");
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
    fsm.trigger(INCREASE);
  else if (current > max)
    fsm.trigger(DECREASE);
  else
    fsm.trigger(IDLE);

  fsm.loop();
}