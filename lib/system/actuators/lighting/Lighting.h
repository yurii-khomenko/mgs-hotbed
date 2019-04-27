#ifndef Lighting_h
#define Lighting_h

#include <Arduino.h>

class Lighting {
public:
  Lighting(u8 pin);

  u8 level = 0;

  void setup(u8 level);

  String metrics();

private:
  u8 pin;
};

#endif