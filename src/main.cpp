#include "../lib/system/System.h"

System sys({
               "greenhouse", "mgs", "hotbed-test",
               "INSIDIUS 2.4GHz", "scalable"
           });

const u8 DHT_SENSOR_PIN = D1;
const u8 HUMIDIFIER_PIN = D4;
const u8 HUMIDIFIER_STATE_PIN = D5;
const u8 VENTILATION_PIN = D6;
const u8 LIGHT_PIN = D7;

void setup(void) {
  sys.setup();
  sys.setupDht(DHT_SENSOR_PIN, DHT22);
//  sys.setupHumidifier(HUMIDIFIER_PIN, HUMIDIFIER_STATE_PIN);
//  sys.setupVentilation(VENTILATION_PIN);
//  sys.setupGigrostat(96, 99);
}

void loop(void) {
  sys.loop();
}