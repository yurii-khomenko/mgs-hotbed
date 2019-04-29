#ifndef Lighting_h
#define Lighting_h

#include <FastLED.h>

class Lighting {
public:
  Lighting(
      u8 pin, u16 ledsNumber,
      const struct CRGB &color = CRGB::White,
      u16 temperature = 20000,
      real32 brightness = 5);
  ~Lighting();

  void setColor(u16 index, const struct CRGB &color, bool force = true);
  void setColor(const struct CRGB &color, bool force = true);
  void setTemperature(u16 levelKelvin, bool force = true);
  void setBrightness(real32 levelPercent, bool force = true);

  String metrics();

private:
  u8 pin;
  u16 ledsNumber;
  u16 temperature;
  real32 brightness;
  CRGB *leds;
};

#endif