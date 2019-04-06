#ifndef ESP8266System_h
#define ESP8266System_h

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

struct Conf {
  
  String groupName;
  String systemName;
  String serviceName;

  String ssid;
  String password;

  u16 serverPort;
};

class ESP8266System {
  public:
    ESP8266System(Conf conf);
    void setup();
    void loop();
    void setupDHT(u8 pin, u8 type);
  private:
    Conf conf;
    void _setupWifi(String ssid, String password);
    void _setupWebServer(u16 serverPort);
    String _getDhtMetrics();
    String _getMetrics();
};

#endif