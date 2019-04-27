#ifndef Lighting_h
#define Lighting_h

#include <Arduino.h>

class Lighting {
public:
  Lighting(u8 pin, u8 ledNum);

  void setup();

//  String metrics();

private:
  u8 pin;
};

#endif