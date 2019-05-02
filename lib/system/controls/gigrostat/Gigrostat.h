#ifndef Gigrostat_h
#define Gigrostat_h

#include "../../sensors/dht/DhtSensor.h"
#include "../../actuators/humidifier/Humidifier.h"
#include "../../actuators/ventilation/Ventilation.h"
#include "../../controls/fsm/Fsm.h"

#define IDLE        0
#define INCREASE    1
#define DECREASE    2

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

class Gigrostat {
public:
  Gigrostat(DhtSensor *dhtSensor, Humidifier *humidifier, Ventilation *ventilation) {
    this->dhtSensor = dhtSensor;
    this->humidifier = humidifier;
    this->ventilation = ventilation;
  }

  void setHumidity(real32 min, real32 max){

    this->min = min;
    this->max = max;

    fsm.clearTransition();

    fsm.addTransition(&idle, &increase, INCREASE, [this] {
      Serial.println("[Gigrostat] Transitioning from idle to increase");
      ventilation->setFlow(0);
      humidifier->setFlow(100);
    });
    fsm.addTransition(&increase, &idle, IDLE, [this] {
      Serial.println("[Gigrostat] Transitioning from increase to idle");
      ventilation->setFlow(0);
      humidifier->setFlow(0);
    });

    fsm.addTransition(&increase, &decrease, DECREASE, [this] {
      Serial.println("[Gigrostat] Transitioning from increase to decrease");
      ventilation->setFlow(100);
      humidifier->setFlow(0);
    });
    fsm.addTransition(&decrease, &increase, INCREASE, [this] {
      Serial.println("[Gigrostat] Transitioning from decrease to increase");
      ventilation->setFlow(0);
      humidifier->setFlow(100);
    });

    fsm.addTransition(&decrease, &idle, IDLE, [this] {
      Serial.println("[Gigrostat] Transitioning from decrease to idle");
      ventilation->setFlow(0);
      humidifier->setFlow(0);
    });
    fsm.addTransition(&idle, &decrease, DECREASE, [this] {
      Serial.println("[Gigrostat] Transitioning from idle to decrease");
      ventilation->setFlow(100);
      humidifier->setFlow(0);
    });
  }

  void loop() {

    const float current = dhtSensor->humidity();

    if (isnan(current))     return;

    if      (current < min) fsm.trigger(INCREASE);
    else if (current > max) fsm.trigger(DECREASE);
    else                    fsm.trigger(IDLE);

    fsm.loop();
  }

private:
  DhtSensor   *dhtSensor;
  Humidifier  *humidifier;
  Ventilation *ventilation;

  real32 min;
  real32 max;
};

#endif