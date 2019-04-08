#include <Humidifier.h>

#include <Arduino.h>

u8 pin;
bool enable;

Humidifier::Humidifier(const u8 pin) {
  this->pin = pin;
  this->enable = false;
}

void Humidifier::on() {
  digitalWrite(pin, LOW);
}

void Humidifier::off() {
  digitalWrite(pin, HIGH);
}