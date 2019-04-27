#include "Lighting.h"
#include <FastLED.h>

Lighting::Lighting(u8 pin, u16 ledNum) {

  this->pin = pin;
  pinMode(pin, OUTPUT);

  CRGB leds[32];

  FastLED.addLeds<WS2812, D7, GRB>(leds, ledNum);
  FastLED.setBrightness(1);
//  FastLED.setTemperature(Tungsten100W);

  fill_solid(leds, 32, CRGB::White);

  FastLED.show();
}