#ifndef ESP8266System_h
#define ESP8266System_h

#include <Arduino.h>

#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

#include <ESP8266WebServer.h>
#include <DhtSensor.h>
#include <Humidifier.h>
#include <Gigrostat.h>

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
    
    ESP8266WebServer* server;
    DhtSensor*        dhtSensor;
    Humidifier*       humidifier;
    Gigrostat*        gigrostat;
    
    void setup();
    void loop();
    void setupPin(u8 pin, u8 mode);
    void setupDHT(u8 pin, u8 type);
    void setupHumidifier(u8 pin, u8 pinStatus);
    void setupGigrostat(const float level, const float accuracy);
    void onPin(u8 pin);
    void offPin(u8 pin);
    void onLed();
    void offLed();
    void withBlink(std::function<void(void)> body);

  private:
    Conf conf;
    String metricPrefix;

    void setupLED();
    void setupWifi();
    void setupOTA();
    void setupWebServer();
    String getMetrics();
    String getDhtMetrics();
};

#endif