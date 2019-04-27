#ifndef System_h
#define System_h

#include <Arduino.h>

#include "infras/wifi/WifiDevice.h"
#include "infras/ota/Ota.h"
#include "infras/mqtt/MqttClient.h"

#include "sensors/dht/DhtSensor.h"

#include "actuators/humidifier/Humidifier.h"
#include "actuators/ventilation/Ventilation.h"
#include <actuators/lighting/Lighting.h>

#include "controls/gigrostat/Gigrostat.h"
#include "controls/metricSender/MetricSender.h"

struct Conf {

  String group;
  String system;
  String service;

  String ssid;
  String password;
};

class System {
public:
  System(const Conf &conf);

  WifiDevice *wifi;
//  NtpClient *ntpClient;
  Ota *ota;
  MqttClient *mqttClient;
  MetricSender *metricSender;

  DhtSensor *dhtSensor;

  Humidifier *humidifier;
  Lighting *lighting;
  Ventilation *ventilation;

  Gigrostat *gigrostat;

  void setup();

  void loop();

  void setupPin(u8 pin, u8 mode);

  void setupDht(u8 pin, u8 type);

  void setupHumidifier(u8 pin, u8 pinStatus);

  void setupLighting(u8 pin, u16 ledNum);

  void setupVentilation(u8 pin);

  void setupGigrostat(real32 min, real32 max);

  void onPin(u8 pin);

  void offPin(u8 pin);

  void onLed();

  void offLed();

  void withBlink(const std::function<void(void)> &body);

private:
  Conf conf;
};

#endif