#include "Humidifier.h"

Humidifier::Humidifier(u8 pin, u8 statePin) {
  this->pin = pin;
  this->statePin = statePin;
  pinMode(pin, OUTPUT_OPEN_DRAIN);
  pinMode(statePin, INPUT);
}

u8 Humidifier::getLevel() {
  return (u8) digitalRead(statePin) ? 100 : 0;
}

void Humidifier::setup(const u8 level) {
  if (level <= 0 && getLevel() >= 100) click();
  else if (level > 0 && getLevel() <= 0) click();
}

String Humidifier::metrics() {
  return String("actuators/humidifier level=") + getLevel();
}

// PRIVATE
//============================================================================>

void Humidifier::click() {
  digitalWrite(pin, LOW);
  delay(200);
  digitalWrite(pin, HIGH);
}