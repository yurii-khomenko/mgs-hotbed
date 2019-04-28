#include "Lighting.h"
#include <FastLED.h>

Lighting::Lighting(u8 pin, u16 ledsNumber) {

  this->pin = pin;
  this->ledsNumber = ledsNumber;
  this->leds = new CRGB[ledsNumber];

  FastLED.addLeds<WS2812, D7, GRB>(leds, ledsNumber);
}

Lighting::~Lighting() {
  setup(CRGB::Black, 0, 0);
  FastLED.show();
  delete leds;
}

// TODO: Set frequency or wave length of light per LED
// TODO: Set temperature in kelvins //http://www.tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/
void Lighting::setup(const struct CRGB &color, u16 temperature, u8 brightness) {

  this->temperature = temperature;
  this->brightness = brightness;

  FastLED.setBrightness(brightness);
  FastLED.setTemperature(OvercastSky);
  fill_solid(leds, ledsNumber, CRGB::White);
  FastLED.show();
}

String Lighting::metrics() {
  return String("actuators/lighting ") +
    "r="           + leds[0].r   + "," +
    "g="           + leds[0].g   + "," +
    "b="           + leds[0].b   + "," +
    "temperature=" + temperature + "," +
    "brightness="  + brightness;
}