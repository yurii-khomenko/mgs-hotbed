#include "../lib/system/System.h"
#include <TaskScheduler.h>

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

Scheduler scheduler;

real32 step = 1;

Task blink(100, TASK_FOREVER, [] {

  if (sys.lighting->getBrightness() < 10)
    step = -step;
  else if (sys.lighting->getBrightness() > 50)
    step = -step;

  sys.lighting->setBrightness(sys.lighting->getBrightness() + step);
});

//, &scheduler, true, [] {
//  pinMode(LED_BUILTIN, OUTPUT);
//  return true;
//});

//Task taskLedOff(500, TASK_FOREVER, [] {
////  sys.lighting->setBrightness(0);
//  digitalWrite(LED_BUILTIN, LOW);
//});



void setup(void) {
  sys.enableSystem();

//  sys.setupDht(DHT_SENSOR_PIN, DHT22);

  sys.enableLighting(LIGHTING_PIN, 32);
  sys.lighting->setColor({255,90,0});
  sys.lighting->setTemperature(1000);
  sys.lighting->setBrightness(10);

  scheduler.init();
  scheduler.addTask(blink);
  scheduler.startNow();
  blink.enable();


//  sys.setupHumidifier(HUMIDIFIER_PIN, HUMIDIFIER_STATE_PIN);
//  sys.setupVentilation(VENTILATION_PIN);
//  sys.setupGigrostat(96, 99);

//
//  Serial.println("Scheduler TEST");
//
//  runner.init();
//  Serial.println("Initialized scheduler");
//
//  runner.addTask(t1);
//  Serial.println("added t1");
//
//  runner.addTask(t2);
//  Serial.println("added t2");
//
//  delay(5000);
//
//  t1.enable();
//  Serial.println("Enabled t1");
//
//  t2.enable();
//  Serial.println("Enabled t2");
}

void loop(void) {
  sys.loop();
  scheduler.execute();
}