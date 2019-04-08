#include <Humidifier.h>
#include <Arduino.h>

Humidifier::Humidifier(const u8 pin, String metricPrefix) {
  this->pin = pin;
  this->metricPrefix = metricPrefix;
  this->enable = false;
}

void Humidifier::on() {
  digitalWrite(pin, LOW);
  enable = true;
}

void Humidifier::off() {
  digitalWrite(pin, HIGH);
  enable = false;
}

String Humidifier::getMetrics() {
  return String(metricPrefix + "humidifier_enable ") + (enable ? 1 : 0) + "\n";
}