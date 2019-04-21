#ifndef ESP8266System_h
#define ESP8266System_h

#include <Arduino.h>

#include <WifiDevice.h>
#include <Ota.h>
#include <MqttClient.h>

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

class Esp8266System {
public:
  Esp8266System(const Conf &conf);

  WifiDevice *wifi;
  Ota *ota;
  MqttClient *mqttClient;
  ESP8266WebServer *server;

  DhtSensor *dhtSensor;
  Humidifier *humidifier;
  Ventilation *ventilation;

  Gigrostat *gigrostat;

  void setup();

  void loop();

  void setupPin(u8 pin, u8 mode);

  void setupDHT(u8 pin, u8 type);

  void setupHumidifier(u8 pin, u8 pinStatus);

  void setupVentilation(u8 pin);

  void setupGigrostat(real32 min, real32 max);

  void onPin(u8 pin);

  void offPin(u8 pin);

  void onLed();

  void offLed();

  void withBlink(const std::function<void(void)> &body);

  String metrics();

private:
  Conf conf;
  String metricPrefix;
};

#endif