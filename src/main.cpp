#include <ESP8266System.h>

ESP8266System sys({

  "greenhouse",
  "mgs",
  "hotbed",

  "INSIDIUS 2.4GHz",
  "scalable"
});



void setup(void) {
  sys.setup();
  sys.setupDHT(D3, DHT22);
  sys.setupHumidifier(D4);
  sys.setupGigrostat(90, 5);
}

void loop(void) {
  sys.loop();
}