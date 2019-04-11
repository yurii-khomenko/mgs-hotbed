#include <Humidifier.h>
#include <Arduino.h>

Humidifier::Humidifier(const u8 pin, String metricPrefix) {
  this->pin = pin;
  this->metricPrefix = metricPrefix;
  this->enable = false;
}

void Humidifier::on() {
  digitalWrite(pin, LOW);
  delay(200);
  digitalWrite(pin, HIGH);
  enable = true;
}

void Humidifier::off() {
  digitalWrite(pin, LOW);
  delay(200);
  digitalWrite(pin, HIGH);
  enable = false;
}

String Humidifier::getMetrics() {
  return String(metricPrefix + "humidifier_enable ") + (enable ? 1 : 0) + "\n";
}

std::vector<String> Humidifier::getMetricsList() {
  return {
    String("humidifier_enable ") + (enable ? 1 : 0) + "\n"
  };
}