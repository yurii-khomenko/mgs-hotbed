#include "Lighting.h"

Lighting::Lighting(u8 pin) {
  this->pin = pin;
  pinMode(pin, OUTPUT);
}

void Lighting::setup(u8 level) {

  this->level = level;
}

String Lighting::metrics() {
  return String("actuators/lighting level=") + level;
}