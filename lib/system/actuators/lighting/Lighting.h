#ifndef Lighting_h
#define Lighting_h

#include <FastLED.h>
#include <ArduinoJson.h>

class Lighting {
public:
  Lighting(u8 pin, u16 ledsNumber) {

    this->pin = pin;
    this->ledsNumber = ledsNumber;

    this->leds = new CRGB[ledsNumber];

    FastLED.addLeds<WS2812, D7, GRB>(leds, ledsNumber);

    Serial.println((String) "[Lighting] enable, GPIO: " + pin);
  }

  ~Lighting() {
    FastLED.clearData();
    delete leds;
    Serial.println("[Lighting] disable");
  }

  void setColor(u16 index, const CRGB &color, bool force = true) {

    if (index <= 0 || index >= ledsNumber) return;

    leds[index] = color;
    if (force) FastLED.show();
  }

  void setColor(const CRGB &color, bool force = true) {
    this->color = color;
    fill_solid(leds, ledsNumber, color);
    if (force) FastLED.show();
  }

  real32 getTemperature() { return temperature; };

  void setTemperature(u16 levelKelvin, bool force = true) {

    //TODO TOP1 create function to convert Kelvin to RGB http://www.tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/

    temperature = levelKelvin;

    if (levelKelvin <= 1900) FastLED.setTemperature(Candle);
    else if (levelKelvin <= 2600) FastLED.setTemperature(Tungsten40W);
    else if (levelKelvin <= 2850) FastLED.setTemperature(Tungsten100W);
    else if (levelKelvin <= 3200) FastLED.setTemperature(Halogen);
    else if (levelKelvin <= 5200) FastLED.setTemperature(CarbonArc);
    else if (levelKelvin <= 5400) FastLED.setTemperature(HighNoonSun);
    else if (levelKelvin <= 6000) FastLED.setTemperature(DirectSunlight);
    else if (levelKelvin <= 7000) FastLED.setTemperature(OvercastSky);
    else FastLED.setTemperature(ClearBlueSky);

    if (force) FastLED.show();
  }

  real32 getBrightness() { return brightness; };

  void setBrightness(real32 levelPercent, bool force = true) {

    if (levelPercent == brightness) return;
    else if (levelPercent <= 0) brightness = 0;
    else if (levelPercent >= 100) brightness = 100;
    else brightness = levelPercent;

    FastLED.setBrightness((u8) (brightness * 2.55));
    if (force) FastLED.show();
  }

  void setConfig(const JsonDocument &config) {

    setColor({
                 config["actuators"]["lighting"]["color"]["r"] | color.r,
                 config["actuators"]["lighting"]["color"]["g"] | color.g,
                 config["actuators"]["lighting"]["color"]["b"] | color.b
             },
                 false);

    setTemperature(config["actuators"]["lighting"]["temperature"] | temperature, false);
    setBrightness( config["actuators"]["lighting"]["brightness"]  | brightness);
  }

  String getState() {
    return String("actuators/lighting ") +
           "r="           + leds[0].r    + "," +
           "g="           + leds[0].g    + "," +
           "b="           + leds[0].b    + "," +
           "temperature=" + temperature  + "," +
           "brightness="  + brightness;
  }

private:
  u8 pin;
  u16 ledsNumber;
  CRGB color;
  u16 temperature;
  real32 brightness;

  CRGB *leds;
};

#endif