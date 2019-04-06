#ifndef ESP8266IOT_h
#define ESP8266IOT_h
 
#include <Arduino.h>

class ESP8266IOT {
public:
 ESP8266IOT(int serialSpeed, char* ssid, char* password);
//  void blink(bool value);  //enable blinking with 1s duration
};
 
#endif