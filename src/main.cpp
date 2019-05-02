#include <System.h>

System sys({
               "greenhouse", "mgs", "hotbed-test",
               "INSIDIUS 2.4GHz", "scalable"
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
}

void loop(void) {
  sys.loop();
}