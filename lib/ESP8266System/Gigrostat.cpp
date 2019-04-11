#include <Gigrostat.h>
#include <Arduino.h>
#include <Fsm.h>

#define IDLE 0
#define INC_HUMIDITY 1
#define DEC_HUMIDITY 2

State stateIdle(
[] {
  Serial.println("Entering stateIdle");
},
[] {
  Serial.print("In stateIdle");
}, 
[] {
  Serial.println("Exiting stateIdle");
});

State stateIncHumidity(
[] {
  Serial.println("Entering stateIncHumidity");
},
[] {
  Serial.print("In stateIncHumidity");
}, 
[] {
  Serial.println("Exiting stateIncHumidity");
});

State stateDecHumidity(
[] {
  Serial.println("Entering stateDecHumidity");
},
[] {
  Serial.print("In stateDecHumidity");
}, 
[] {
  Serial.println("Exiting stateDecHumidity");
});

Fsm fsm(&stateIdle);

Gigrostat::Gigrostat(DHT *dht, Humidifier *humidifier, String metricPrefix) {
  this->dht = dht;
  this->humidifier = humidifier;
  this->metricPrefix = metricPrefix;
}

void Gigrostat::setup(const float min, const float max) {
  
  this->min = min;
  this->max = max;

  fsm.add_transition(&stateIdle, &stateIncHumidity, INC_HUMIDITY, [this] () {
    Serial.println("Transitioning from stateIdle to stateIncHumidity");
    // ventilation->off();
    humidifier->on();
  });
  fsm.add_transition(&stateIncHumidity, &stateIdle, IDLE, [this] {
    Serial.println("Transitioning from stateIncHumidity to stateIdle");
    // ventilation->off();
    humidifier->off();
  });

  fsm.add_transition(&stateIncHumidity, &stateDecHumidity, DEC_HUMIDITY, [this] {
    Serial.println("Transitioning from stateIncHumidity to stateDecHumidity");
    humidifier->off();
    // ventilation->on();
  });
  fsm.add_transition(&stateDecHumidity, &stateIncHumidity, INC_HUMIDITY, [this] {
    Serial.println("Transitioning from stateDecHumidity to stateIncHumidity");
    // ventilation->off();
    humidifier->on();
  });

  fsm.add_transition(&stateDecHumidity, &stateIdle, IDLE, [this] {
    Serial.println("Transitioning from stateDecHumidity to stateIdle");
    humidifier->off();
    // ventilation->off();
  });
  fsm.add_transition(&stateIdle, &stateDecHumidity, DEC_HUMIDITY, [this] {
    Serial.println("Transitioning from stateIdle to stateDecHumidity");
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

  if(current < min) {

    // humidifier->on();
    fsm.trigger(INC_HUMIDITY);
    Serial.print("[Gigrostat] humidifier->on(), current: ");
    Serial.println(current);

  } else if (current > max) {

    fsm.trigger(DEC_HUMIDITY);

    // humidifier->off();
    Serial.print("[Gigrostat] humidifier->off(), current: ");
    Serial.println(current);

  } else {

    fsm.trigger(IDLE);

    Serial.print("[Gigrostat] humidity ok, min: ");
    Serial.print(min);

    Serial.print(" current: ");
    Serial.print(current);

    Serial.print(" max: ");
    Serial.println(max);
  }

  fsm.run_machine();
}