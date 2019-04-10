#include <ESP8266System.h>

ESP8266System sys({

  "greenhouse",
  "mgs",
  "hotbed",

  "INSIDIUS 2.4GHz",
  "scalable"
});

#define DHT_SENSOR D3
#define HUMIDIFIER D4

void setup(void) {
  sys.setup();
  sys.setupDHT(DHT_SENSOR, DHT22);
  sys.setupHumidifier(HUMIDIFIER);
  sys.setupGigrostat(80, 5);
}

void loop(void) {
  sys.loop();
}