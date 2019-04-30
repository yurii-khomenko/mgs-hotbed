#include "Lighting.h"
#include <FastLED.h>

Lighting::Lighting(
    u8 pin, u16 ledsNumber,
    const CRGB& color,
    u16 temperature,
    real32 brightness) {

  this->pin = pin;
  this->ledsNumber = ledsNumber;
  this->color = color;

  this->leds = new CRGB[ledsNumber];

  FastLED.addLeds<WS2812, D7, GRB>(leds, ledsNumber);
  setColor(color);
  setTemperature(temperature);
  setBrightness(brightness);
}

Lighting::~Lighting() {
  setBrightness(0);
  FastLED.show();
  delete leds;
}

String Lighting::metrics() {
  return String("actuators/lighting ") +
         "r="           + leds[0].r   + "," +
         "g="           + leds[0].g   + "," +
         "b="           + leds[0].b   + "," +
         "temperature=" + temperature + "," +
         "brightness="  + brightness;
}

void Lighting::setState(const DynamicJsonDocument &state) {

  setColor({
               state["actuators"]["lighting"]["color"]["r"] | color.r,
               state["actuators"]["lighting"]["color"]["g"] | color.g,
               state["actuators"]["lighting"]["color"]["b"] | color.b
           });

  setTemperature(state["actuators"]["lighting"]["temperature"] | temperature);
  setBrightness (state["actuators"]["lighting"]["brightness"]  | brightness);
}

void Lighting::setColor(u16 index, const struct CRGB &color) {

  if (index <= 0 || index >= ledsNumber) return;

  leds[index] = color;
  FastLED.show();
}

void Lighting::setColor(const CRGB &color) {
  this->color = color;
  fill_solid(leds, ledsNumber, color);
  FastLED.show();
}


void Lighting::setTemperature(u16 levelKelvin) {

  //TODO TOP1 create function to convert Kelvin to RGB http://www.tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/

  temperature = levelKelvin;

  if      (levelKelvin <= 1900)   FastLED.setTemperature(Candle);
  else if (levelKelvin <= 2600)   FastLED.setTemperature(Tungsten40W);
  else if (levelKelvin <= 2850)   FastLED.setTemperature(Tungsten100W);
  else if (levelKelvin <= 3200)   FastLED.setTemperature(Halogen);
  else if (levelKelvin <= 5200)   FastLED.setTemperature(CarbonArc);
  else if (levelKelvin <= 5400)   FastLED.setTemperature(HighNoonSun);
  else if (levelKelvin <= 6000)   FastLED.setTemperature(DirectSunlight);
  else if (levelKelvin <= 7000)   FastLED.setTemperature(OvercastSky);
  else                            FastLED.setTemperature(ClearBlueSky);

  FastLED.show();
}

void Lighting::setBrightness(real32 levelPercent) {

  if      (levelPercent == brightness) return;
  else if (levelPercent <= 0)          brightness = 0;
  else if (levelPercent >= 100)        brightness = 100;
  else                                 brightness = levelPercent;

  FastLED.setBrightness((u8) (brightness * 2.55));
  FastLED.show();
}
