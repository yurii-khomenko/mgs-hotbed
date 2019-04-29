#include "../lib/system/System.h"

System sys({
               "greenhouse", "mgs", "hotbed",
               "INSIDIUS 2.4GHz", "scalable"
           });

const u8 DHT_SENSOR_PIN = D1;
const u8 HUMIDIFIER_PIN = D4;
const u8 HUMIDIFIER_STATE_PIN = D5;
const u8 VENTILATION_PIN = D6;
const u8 LIGHTING_PIN = D7;





//#include <TaskScheduler.h>

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

  sys.setupDht(DHT_SENSOR_PIN, DHT22);

  sys.setupLighting(LIGHTING_PIN, 48);

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
//  runner.execute();
}