#include "Ventilation.h"

Ventilation::Ventilation(u8 pin) {
  this->pin = pin;
  pinMode(pin, OUTPUT);
}

String Ventilation::metrics() {
  return String("actuators/ventilation flow=") + flow;
}

void Ventilation::setState(const DynamicJsonDocument &state) {
  setFlow(state["actuators"]["ventilation"]["flow"] | flow);
}

void Ventilation::setFlow(real32 levelPercent) {

  if      (levelPercent == flow) return;
  else if (levelPercent <= 0)          flow = 0;
  else if (levelPercent >= 100)        flow = 100;
  else                                 flow = levelPercent;

  if (flow <= 0)
    digitalWrite(pin, LOW);
  else
    digitalWrite(pin, HIGH);
}