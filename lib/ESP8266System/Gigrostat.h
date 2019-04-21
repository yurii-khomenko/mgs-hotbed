#ifndef Gigrostat_h
#define Gigrostat_h

#include "../sensors/DhtSensor.h"
#include "../actuators/Humidifier.h"
#include "../actuators/Ventilation.h"

class Gigrostat {
public:
  Gigrostat(DhtSensor *dhtSensor, Humidifier *humidifier, Ventilation *ventilation);

  void setup(real32 min, real32 max);

  void loop();

private:
  DhtSensor *dhtSensor;
  Humidifier *humidifier;
  Ventilation *ventilation;

  real32 min;
  real32 max;
};

#endif