#ifndef Lighting_h
#define Lighting_h

#include <FastLED.h>
#include <ArduinoJson.h>

class Lighting {
public:
  Lighting(
      u8 pin, u16 ledsNumber,
      const CRGB& color = CRGB::White,
      u16 temperature = 20000,
      real32 brightness = 0);
  ~Lighting();

  String metrics();

  void setColor(u16 index, const struct CRGB &color);
  void setColor(const CRGB &color);
  void setTemperature(u16 levelKelvin);
  void setBrightness(real32 levelPercent);

  void setState(const DynamicJsonDocument &state);

private:
  u8 pin;
  u16 ledsNumber;
  CRGB color;
  u16 temperature;
  real32 brightness;

  CRGB *leds;
};

#endif