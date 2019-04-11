#include <ESP8266System.h>
// #include <TimerOne.h>

ESP8266System sys({

  "greenhouse",
  "mgs",
  "hotbed",

  "INSIDIUS 2.4GHz",
  "scalable"
});

#define DHT_SENSOR_PIN D3
#define HUMIDIFIER_PIN D4
#define HUMIDIFIER_STATE_PIN D5

void setup(void) {
  sys.setup();
  sys.setupDHT(DHT_SENSOR_PIN, DHT22);
  sys.setupHumidifier(HUMIDIFIER_PIN, HUMIDIFIER_STATE_PIN);
  sys.setupGigrostat(98, 1);
}

void loop(void) {
  Serial.println(digitalRead(D5));
  sys.loop();
}