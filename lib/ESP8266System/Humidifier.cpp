#include <Humidifier.h>

Humidifier::Humidifier(u8 pin, u8 statePin, String metricPrefix) {
  this->pin = pin;
  this->statePin = statePin;
  this->metricPrefix = metricPrefix;
  pinMode(pin, OUTPUT_OPEN_DRAIN);
  pinMode(statePin, INPUT);
}

void Humidifier::on() {
  if (!isOn()) click();
}

void Humidifier::off() {
  if (isOn()) click();
}

bool Humidifier::isOn() {
  return digitalRead(statePin);
}

String Humidifier::getMetrics() {
  return String(metricPrefix) + "humidifier_enable " + (isOn() ? 1 : 0) + "\n";
}

void Humidifier::click() {
  digitalWrite(pin, LOW);
  delay(200);
  digitalWrite(pin, HIGH);
}