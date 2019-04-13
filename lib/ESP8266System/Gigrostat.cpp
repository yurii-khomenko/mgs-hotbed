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
NULL, 
[] {
  Serial.println("[Gigrostat] Exiting idle");
});

State increase(
[] {
  Serial.println("[Gigrostat] Entering increase");
},
NULL, 
[] {
  Serial.println("[Gigrostat] Exiting increase");
});

State decrease(
[] {
  Serial.println("[Gigrostat] Entering decrease");
},
NULL, 
[] {
  Serial.println("[Gigrostat] Exiting decrease");
});

Fsm fsm(&idle);

Gigrostat::Gigrostat(DhtSensor* dhtSensor, Humidifier* humidifier, String metricPrefix) {
  this->dhtSensor = dhtSensor;
  this->humidifier = humidifier;
  this->metricPrefix = metricPrefix;
}

void Gigrostat::setup(const real32 min, const real32 max) {
  
  this->min = min;
  this->max = max;

  fsm.clearTransition();

  fsm.addTransition(&idle, &increase, INCREASE, [this] () {
    Serial.println("[Gigrostat] Transitioning from idle to increase");
    // ventilation->off();
    humidifier->on();
  });
  fsm.addTransition(&increase, &idle, IDLE, [this] {
    Serial.println("[Gigrostat] Transitioning from increase to idle");
    // ventilation->off();
    humidifier->off();
  });

  fsm.addTransition(&increase, &decrease, DECREASE, [this] {
    Serial.println("[Gigrostat] Transitioning from increase to decrease");
    humidifier->off();
    // ventilation->on();
  });
  fsm.addTransition(&decrease, &increase, INCREASE, [this] {
    Serial.println("[Gigrostat] Transitioning from decrease to increase");
    // ventilation->off();
    humidifier->on();
  });

  fsm.addTransition(&decrease, &idle, IDLE, [this] {
    Serial.println("[Gigrostat] Transitioning from decrease to idle");
    humidifier->off();
    // ventilation->off();
  });
  fsm.addTransition(&idle, &decrease, DECREASE, [this] {
    Serial.println("[Gigrostat] Transitioning from idle to decrease");
    humidifier->off();
    // ventilation->on();
  });
}

void Gigrostat::loop() {

  const float current = dhtSensor->getHumidity();
  
  if(isnan(current)) return;

  if(current < min)
    fsm.trigger(INCREASE);
  else if (current > max)
    fsm.trigger(DECREASE);
  else
    fsm.trigger(IDLE);

  fsm.loop();
}