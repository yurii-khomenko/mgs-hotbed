#ifndef ESP8266System_h
#define ESP8266System_h

#include <Arduino.h>

#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

#include <ESP8266WebServer.h>
#include <DHT.h>


struct Conf {

  String groupName;
  String systemName;
  String serviceName;

  String ssid;
  String password;
};

class ESP8266System {
  public:
    ESP8266System(Conf conf);
    void setupDHT(u8 pin, u8 type);
    void ledOn();
    void ledOff();
    void withBlink(std::function<void(void)> body);
    void setup();
    void loop();
  private:
    Conf conf;
    void _setupLED();
    void _setupWifi();
    void _setupOTA();
    void _setupWebServer();
    String _getDhtMetrics();
    String _getMetrics();
};

#endif