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

real32 step = 1;

Task blink(100, TASK_FOREVER, [] {

  if (sys.lighting->getBrightness() < 10)
    step = -step;
  else if (sys.lighting->getBrightness() > 50)
    step = -step;

  sys.lighting->setBrightness(sys.lighting->getBrightness() + step);
});

void setup(void) {

  sys.enableSystem();

  sys.enableDht(DHT_SENSOR_PIN, DHT22);

  sys.enableLighting(LIGHTING_PIN, 32);
  sys.lighting->setColor({255,90,0});
  sys.lighting->setTemperature(1000);
  sys.lighting->setBrightness(10);

  sys.scheduler.addTask(blink);
  blink.enable();


//  sys.enableHumidifier(HUMIDIFIER_PIN, HUMIDIFIER_STATE_PIN);
//  sys.enableVentilation(VENTILATION_PIN);
//  sys.enableGigrostat(96, 99);
}

void loop(void) {
  sys.loop();
}