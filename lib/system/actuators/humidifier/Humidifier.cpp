#include "Humidifier.h"

Humidifier::Humidifier(u8 pin, u8 statePin) {
  this->pin = pin;
  this->statePin = statePin;
  pinMode(pin, OUTPUT_OPEN_DRAIN);
  pinMode(statePin, INPUT);
}

String Humidifier::metrics() {
  return String("actuators/humidifier flow=") + getFlow();
}

void Humidifier::setState(const DynamicJsonDocument &state) {
  setFlow(state["actuators"]["humidifier"]["flow"] | getFlow());
}

u8 Humidifier::getFlow() {
  return (u8) digitalRead(statePin) ? 100 : 0;
}

void Humidifier::setFlow(u8 level) {
  if (level <= 0 && getFlow() >= 100) click();
  else if (level > 0 && getFlow() <= 0) click();
}

// PRIVATE
//============================================================================>

void Humidifier::click() {
  digitalWrite(pin, LOW);
  delay(200);
  digitalWrite(pin, HIGH);
}