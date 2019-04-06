#include <ESP8266System.h>

ESP8266System sys({
  
  "greenhouse",
  "mgs",
  "hotbed",

  "INSIDIUS 2.4GHz",
  "scalable",
  D9
});

void setup(void) {
  sys.setup();
  // sys.setupDHT(D2, DHT22);
}

void loop(void) {
  sys.loop();
}