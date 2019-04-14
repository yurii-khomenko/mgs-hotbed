#include <ESP8266System.h>

ESP8266System sys({
                          "greenhouse", "mgs", "hotbed",
                          "INSIDIUS 2.4GHz", "scalable"
                  });

#define DHT_SENSOR_PIN        D3
#define HUMIDIFIER_PIN        D4
#define HUMIDIFIER_STATE_PIN  D5
#define VENTILATION_PIN       D6

void setup(void) {
    sys.setup();
    sys.setupDHT(DHT_SENSOR_PIN, DHT22);
    sys.setupHumidifier(HUMIDIFIER_PIN, HUMIDIFIER_STATE_PIN);
    sys.setupVentilation(VENTILATION_PIN);
    sys.setupGigrostat(96, 99);
}

void loop(void) {
    sys.loop();
}