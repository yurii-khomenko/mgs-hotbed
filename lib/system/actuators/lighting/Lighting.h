#ifndef Lighting_h
#define Lighting_h

#include <FastLED.h>

class Lighting {
public:
  Lighting(u8 pin, u16 ledNum);
  ~Lighting();

  void setup(const struct CRGB &color, u16 temp, u8 brightness);

  String metrics();

private:
  u8 pin;
  u16 ledNum;
  CRGB *leds;
};

#endif