#include <Humidifier.h>

#include <Arduino.h>

u8 pin;

Humidifier::Humidifier(const u8 pin) {
  this->pin = pin;
}

void Humidifier::on() {
  digitalWrite(pin, LOW);
}

void Humidifier::off() {
  digitalWrite(pin, HIGH);
}