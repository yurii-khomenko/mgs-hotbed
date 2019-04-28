#include "Ventilation.h"

Ventilation::Ventilation(u8 pin) {
  this->pin = pin;
  pinMode(pin, OUTPUT);
}

void Ventilation::setup(u8 level) { // TODO: move to real32

  this->level = level;

  if (level <= 0)
    digitalWrite(pin, LOW);
  else
    digitalWrite(pin, HIGH);
}

String Ventilation::metrics() {
  return String("actuators/ventilation level=") + level;
}