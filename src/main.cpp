#include "../lib/system/System.h"

System sys({
               "greenhouse", "mgs", "hotbed-test",
               "INSIDIUS 2.4GHz", "scalable"
           });

const u8 DHT_SENSOR_PIN = D1;
const u8 HUMIDIFIER_PIN = D4;
const u8 HUMIDIFIER_STATE_PIN = D5;
const u8 VENTILATION_PIN = D6;
const u8 LIGHTING_PIN = D7;

// move to lib System and config
// constructor to define
// setHumidity(hum, gigro);
// all setter setHumidi
// and set all as mqtt packet. we have json config in a packet.

Task taskLighting(1000, TASK_FOREVER, [] {

  const u8 startLightHours = 00;
  const u8 startLightMinutes = 17;
  const u8 startLightSeconds = 00;

  const u8 endLightHours = 00;
  const u8 endLightMinutes = 05;
  const u8 endLightSeconds = 59;

  const u8 hh = sys.ntpClient->getHours();
  const u8 mm = sys.ntpClient->getMinutes();
  const u8 ss = sys.ntpClient->getSeconds();

  Serial.println(hh);
  Serial.println(mm);
  Serial.println(ss);

  if (startLightHours   <= hh && hh <= endLightHours &&
      startLightMinutes <= mm && mm <= endLightMinutes &&
      startLightSeconds <= ss && ss <= endLightSeconds) {

    Serial.println("hit");

    sys.lighting->setTemperature(20000);
    sys.lighting->setColor(CRGB::White);
    sys.lighting->setBrightness(10);
  } else {
    Serial.println("no hit");
    sys.lighting->setBrightness(0);
  }
});

void setup(void) {

  sys.enableSystem();

  sys.enableDht(DHT_SENSOR_PIN, DHT22);

  sys.enableLighting(LIGHTING_PIN, 32);
//  sys.lighting->setColor({255,90,0});
//  sys.lighting->setTemperature(1000);
//  sys.lighting->setBrightness(10);

  sys.scheduler.addTask(taskLighting);
  taskLighting.enable();

//  sys.enableHumidifier(HUMIDIFIER_PIN, HUMIDIFIER_STATE_PIN);
//  sys.enableVentilation(VENTILATION_PIN);
//  sys.enableGigrostat(96, 99);
}

void loop(void) {
  sys.loop();
}