#include "Lighting.h"
#include <FastLED.h>

Lighting::Lighting(u8 pin, u16 ledNum) {

  this->pin = pin;
  this->ledNum = ledNum;
  this->leds = new CRGB[ledNum];

  FastLED.addLeds<WS2812, D7, GRB>(leds, ledNum);
}

Lighting::~Lighting() {
  setup(CRGB::Black, 0, 0);
  FastLED.show();
  delete leds;
}

void Lighting::setup(const struct CRGB &color, u16 temp, u8 brightness) {
  FastLED.setBrightness(brightness);
  FastLED.setTemperature(OvercastSky); //http://www.tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/
  fill_solid(leds, ledNum, CRGB::White);
  FastLED.show();
}