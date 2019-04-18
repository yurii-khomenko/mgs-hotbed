#include <Gigrostat.h>
#include <Fsm.h>

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

Gigrostat::Gigrostat(DhtSensor *dhtSensor, Humidifier *humidifier, Ventilation *ventilation) {
    this->dhtSensor = dhtSensor;
    this->humidifier = humidifier;
    this->ventilation = ventilation;
}

void Gigrostat::setup(const real32 min, const real32 max) {

    this->min = min;
    this->max = max;

    fsm.clearTransition();

    fsm.addTransition(&idle, &increase, INCREASE, [this] {
        Serial.println("[Gigrostat] Transitioning from idle to increase");
        ventilation->setup(0);
        humidifier->setup(100);
    });
    fsm.addTransition(&increase, &idle, IDLE, [this] {
        Serial.println("[Gigrostat] Transitioning from increase to idle");
        ventilation->setup(0);
        humidifier->setup(0);
    });

    fsm.addTransition(&increase, &decrease, DECREASE, [this] {
        Serial.println("[Gigrostat] Transitioning from increase to decrease");
        ventilation->setup(100);
        humidifier->setup(0);
    });
    fsm.addTransition(&decrease, &increase, INCREASE, [this] {
        Serial.println("[Gigrostat] Transitioning from decrease to increase");
        ventilation->setup(0);
        humidifier->setup(100);
    });

    fsm.addTransition(&decrease, &idle, IDLE, [this] {
        Serial.println("[Gigrostat] Transitioning from decrease to idle");
        ventilation->setup(0);
        humidifier->setup(0);
    });
    fsm.addTransition(&idle, &decrease, DECREASE, [this] {
        Serial.println("[Gigrostat] Transitioning from idle to decrease");
        ventilation->setup(100);
        humidifier->setup(0);
    });
}

void Gigrostat::loop() {

    const float current = dhtSensor->humidity();

    if (isnan(current)) return;

    if (current < min)
        fsm.trigger(INCREASE);
    else if (current > max)
        fsm.trigger(DECREASE);
    else
        fsm.trigger(IDLE);

    fsm.loop();
}