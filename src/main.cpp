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

void setup(void) {
  sys.setup();

  sys.setupDht(DHT_SENSOR_PIN, DHT22);

  sys.setupLighting(LIGHTING_PIN, 48);

//  sys.setupHumidifier(HUMIDIFIER_PIN, HUMIDIFIER_STATE_PIN);
//  sys.setupVentilation(VENTILATION_PIN);
//  sys.setupGigrostat(96, 99);
}

void loop(void) {
  sys.loop();
}

//// constants won't change. Used here to set a pin number :
//const int ledPin =  13;      // the number of the LED pin
//
//// Variables will change :
//int ledState = LOW;             // ledState used to set the LED
//
//// Generally, you should use "unsigned long" for variables that hold time
//// The value will quickly become too large for an int to store
//unsigned long previousMillis = 0;        // will store last time LED was updated
//
//// constants won't change :
//const unsigned long interval = 12UL*60UL*60UL*1000UL;   // Change the LED every 12 hours
//
//void setup() {
//  // set the digital pin as output:
//  pinMode(ledPin, OUTPUT);
//}
//
//void loop() {
//  // here is where you'd put code that needs to be running all the time.
//
//  // check to see if it's time to toggle the LED; that is, if the
//  // difference between the current time and last time you blinked
//  // the LED is bigger than the interval at which you want to
//  // blink the LED.
//  unsigned long currentMillis = millis();
//
//  if (currentMillis - previousMillis >= interval) {
//    // save the time you should have toggled the LED
//    previousMillis += interval;
//
//    // if the LED is off turn it on and vice-versa:
//    if (ledState == LOW) {
//      ledState = HIGH;
//    } else {
//      ledState = LOW;
//    }
//
//    // set the LED with the ledState of the variable:
//    digitalWrite(ledPin, ledState);
//  }
//}