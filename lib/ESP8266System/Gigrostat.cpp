#include <Gigrostat.h>
#include <Arduino.h>
#include <Fsm.h>

// State machine variables
#define IDLE 0
#define INCREASE_HUMIDITY 1
#define DECREASE_HUMIDITY 2

State stateIdle(
[] {
  Serial.println("Entering stateIdle");
},
[] {
  Serial.println("In stateIdle");
}, 
[] {
  Serial.println("Exiting stateIdle");
});

State stateIncreaceHumidity(
[] {
  Serial.println("Entering stateIncreaceHumidity");
},
[] {
  Serial.println("In stateIncreaceHumidity");
}, 
[] {
  Serial.println("Exiting stateIncreaceHumidity");
});

State stateDecreaceHumidity(
[] {
  Serial.println("Entering stateDecreaceHumidity");
},
[] {
  Serial.println("In stateDecreaceHumidity");
}, 
[] {
  Serial.println("Exiting stateDecreaceHumidity");
});

Fsm fsm(&stateIdle);

Gigrostat::Gigrostat(DHT *dht, Humidifier *humidifier, String metricPrefix) {
  this->dht = dht;
  this->humidifier = humidifier;
  this->metricPrefix = metricPrefix;
}

void Gigrostat::setup(const float humidity, const float accuracy) {
  
  this->humidity = humidity;
  this->accuracy = accuracy;

  fsm.add_transition(&stateIdle, &stateIncreaceHumidity, INCREASE_HUMIDITY, [this] () {
    Serial.println("Transitioning from stateIdle to stateIncreaceHumidity");
    // ventilation->off();
    humidifier->on();
  });
  fsm.add_transition(&stateIncreaceHumidity, &stateIdle, IDLE, [this] {
    Serial.println("Transitioning from stateIncreaceHumidity to stateIdle");
    // ventilation->off();
    humidifier->off();
  });

  fsm.add_transition(&stateIncreaceHumidity, &stateDecreaceHumidity, DECREASE_HUMIDITY, [this] {
    Serial.println("Transitioning from stateIncreaceHumidity to stateDecreaceHumidity");
    humidifier->off();
    // ventilation->on();
  });
  fsm.add_transition(&stateDecreaceHumidity, &stateIncreaceHumidity, INCREASE_HUMIDITY, [this] {
    Serial.println("Transitioning from stateDecreaceHumidity to stateIncreaceHumidity");
    // ventilation->off();
    humidifier->on();
  });

  fsm.add_transition(&stateDecreaceHumidity, &stateIdle, IDLE, [this] {
    Serial.println("Transitioning from stateDecreaceHumidity to stateIdle");
    humidifier->off();
    // ventilation->off();
  });
  fsm.add_transition(&stateIdle, &stateDecreaceHumidity, DECREASE_HUMIDITY, [this] {
    Serial.println("Transitioning from stateIdle to stateDecreaceHumidity");
    // humidifier->off();
    // ventilation->on();
  });
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

    // humidifier->on();
    fsm.trigger(INCREASE_HUMIDITY);
    Serial.print("[Gigrostat] humidifier->on(), current: ");
    Serial.println(current);

  } else if (current > max) {

    fsm.trigger(DECREASE_HUMIDITY);

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