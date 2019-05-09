#include "../lib/system/System.h"

System sys({
               "greenhouse", "mgs", "hotbed",
               "cbi", "gogolang"
           });

const u8 DHT_SENSOR_PIN = D1;
const u8 HUMIDIFIER_PIN = D4;
const u8 HUMIDIFIER_STATE_PIN = D5;
const u8 VENTILATION_PIN = D6;
const u8 LIGHTING_PIN = D7;

void setup(void) {

  sys.enableSystem();

  sys.enableDht(DHT_SENSOR_PIN, DHT22);

  sys.enableLighting(LIGHTING_PIN, 150);
  sys.lighting->setColor({255,255,255});
  sys.lighting->setTemperature(7000);
  sys.lighting->setBrightness(20);
}

void loop(void) {
  sys.loop();
}