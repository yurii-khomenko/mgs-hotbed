#ifndef Lighting_h
#define Lighting_h

#include <FastLED.h>

class Lighting {
public:
  Lighting(u8 pin, u16 ledsNumber);
  ~Lighting();

  void setup(const struct CRGB &color, u16 temperature, u8 brightness);
  void setBrightness(u8 level);

  String metrics();

private:
  u8 pin;
  u16 ledsNumber;
  u16 temperature;
  u8 brightness;
  CRGB *leds;
};

#endif