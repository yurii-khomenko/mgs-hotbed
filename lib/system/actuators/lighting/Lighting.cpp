#include "Lighting.h"
#include <FastLED.h>

Lighting::Lighting(u8 pin, u8 ledNum) {

  this->pin = pin;
  pinMode(pin, OUTPUT);

  CRGB leds[ledNum];

  FastLED.addLeds<WS2812, D7, RGB>(leds, ledNum);

  leds[0] = CRGB::Red;
  leds[1] = CRGB::Green;
  leds[2] = CRGB::Blue;

  FastLED.show();
}

void Lighting::setup() {



}

//String Lighting::metrics() {
//  return String("actuators/lighting level=") + level;
//}