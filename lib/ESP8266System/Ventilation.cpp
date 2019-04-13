#include <Ventilation.h>

Ventilation::Ventilation(u8 pin, String metricPrefix) {
  this->pin = pin;
  this->metricPrefix = metricPrefix;
  pinMode(pin, OUTPUT);
}

void Ventilation::setup(u8 level) {
  this->level = level;
  if (level == 0) 
    digitalWrite(pin, HIGH);
  else
    digitalWrite(pin, LOW);
}

String Ventilation::metrics() {
  return String(metricPrefix) + "ventilation_level " + (level ? 1 : 0) + "\n";
}