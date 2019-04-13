#include <Ventilation.h>

Ventilation::Ventilation(u8 pin, String metricPrefix) {
  this->pin = pin;
  this->metricPrefix = metricPrefix;
  pinMode(pin, OUTPUT);
}

void Ventilation::setup(u8 level) {
  this->level = level;
  if (level <= 0) 
    digitalWrite(pin, LOW);
  else
    digitalWrite(pin, HIGH);
}

String Ventilation::metrics() {
  return String(metricPrefix) + "ventilation_level " + level + "\n";
}