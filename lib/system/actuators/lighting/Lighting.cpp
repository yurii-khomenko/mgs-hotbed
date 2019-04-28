#include "Lighting.h"
#include <FastLED.h>

Lighting::Lighting(
    u8 pin, u16 ledsNumber,
    const struct CRGB &color,
    u16 temperature,
    real32 brightness) {

  this->pin = pin;
  this->ledsNumber = ledsNumber;
  this->leds = new CRGB[ledsNumber];

  const u8 p = D7;

  FastLED.addLeds<WS2812, p, GRB>(leds, ledsNumber);
  setColor(color);
  setTemperature(temperature);
  setBrightness(brightness);
}

Lighting::~Lighting() {
  setBrightness(0);
  FastLED.show();
  delete leds;
}

// TODO: Set frequency or wave length of light per LED
// TODO: Set temperature in kelvins //http://www.tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/

void Lighting::setColor(u16 index, const struct CRGB &color, bool force) {

  if (index <= 0 || index >= ledsNumber) return;

  leds[index] = color;
  if (force) FastLED.show();
}

void Lighting::setColor(const struct CRGB &color, bool force) {
  fill_solid(leds, ledsNumber, color);
  if (force) FastLED.show();
}

void Lighting::setTemperature(u16 levelKelvin, bool force) { //TODO TOP1 create function to convert Kelvin to RGB

  temperature = levelKelvin;

  if      (levelKelvin <= 1900)   FastLED.setTemperature(Candle);
  else if (levelKelvin <= 2600)   FastLED.setTemperature(Tungsten40W);
  else if (levelKelvin <= 2850)   FastLED.setTemperature(Tungsten100W);
  else if (levelKelvin <= 3200)   FastLED.setTemperature(Halogen);
  else if (levelKelvin <= 5200)   FastLED.setTemperature(CarbonArc);
  else if (levelKelvin <= 5400)   FastLED.setTemperature(HighNoonSun);
  else if (levelKelvin <= 6000)   FastLED.setTemperature(DirectSunlight);
  else if (levelKelvin <= 7000)   FastLED.setTemperature(OvercastSky);
  else if (levelKelvin <= 20000)  FastLED.setTemperature(ClearBlueSky);

  if (force) FastLED.show();
}

void Lighting::setBrightness(real32 levelPercent, bool force) {

  if      (levelPercent == brightness) return;
  else if (levelPercent <= 0)          brightness = 0;
  else if (levelPercent >= 100)        brightness = 100;
  else                                 brightness = levelPercent;

  FastLED.setBrightness((u8) (brightness * 2.55));
  if (force) FastLED.show();
}

String Lighting::metrics() {
  return String("actuators/lighting ") +
    "r="           + leds[0].r   + "," +
    "g="           + leds[0].g   + "," +
    "b="           + leds[0].b   + "," +
    "temperature=" + temperature + "," +
    "brightness="  + brightness;
}