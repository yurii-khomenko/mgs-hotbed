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
// setup(hum, gigro);
// all setter setHumidi
// and set all as mqtt packet. we have json config in a packet.

Scheduler scheduler;

//bool on = false;
//
//Task blink(500, TASK_FOREVER, [] {
//
//  Serial.println("hit");
//  Serial.println(digitalRead(LED_BUILTIN));
//
//  sys.lighting->getBrightness() == 0.0 ?
//  sys.lighting->setBrightness(0) :
//  sys.lighting->setBrightness(0);
//});

//, &scheduler, true, [] {
//  pinMode(LED_BUILTIN, OUTPUT);
//  return true;
//});

//Task taskLedOff(500, TASK_FOREVER, [] {
////  sys.lighting->setBrightness(0);
//  digitalWrite(LED_BUILTIN, LOW);
//});



//// Callback methods prototypes
//void t1Callback();
//void t2Callback();
//void t3Callback();

//Tasks
//Task t4();
//Task t1(2000, 10, &t1Callback);
//Task t2(3000, TASK_FOREVER, &t2Callback);
//Task t3(5000, TASK_FOREVER, &t3Callback);
//
//Scheduler runner;

//
//void t1Callback() {
//  Serial.print("t1: ");
//  Serial.println(millis());
//
//  if (t1.isFirstIteration()) {
//    runner.addTask(t3);
//    t3.enable();
//    Serial.println("t1: enabled t3 and added to the chain");
//  }
//
//  if (t1.isLastIteration()) {
//    t3.disable();
//    runner.deleteTask(t3);
//    t2.setInterval(500);
//    Serial.println("t1: disable t3 and delete it from the chain. t2 interval set to 500");
//  }
//}
//
//void t2Callback() {
//  Serial.print("t2: ");
//  Serial.println(millis());
//}
//
//void t3Callback() {
//  Serial.print("t3: ");
//  Serial.println(millis());
//}





void setup(void) {
  sys.setup();

//  sys.setupDht(DHT_SENSOR_PIN, DHT22);

  sys.setupLighting(LIGHTING_PIN, 48);
  sys.lighting->setColor(CRGB::White);
//  sys.lighting->setBrightness(10);

//  scheduler.init();
//  scheduler.addTask(blink);
//  scheduler.startNow();


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