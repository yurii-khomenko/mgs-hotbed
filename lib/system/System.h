#ifndef System_h
#define System_h

#include <Arduino.h>

#include "infra/wifi/WifiDevice.h"
#include "infra/ota/Ota.h"
#include "infra/mqtt/MqttClient.h"

#include "sensors/dht/DhtSensor.h"
#include "actuators/humidifier/Humidifier.h"
#include "control/gigrostat/Gigrostat.h"

struct Conf {

  String groupName;
  String systemName;
  String serviceName;

  String ssid;
  String password;
};

class System {
public:
  System(const Conf &conf);

  WifiDevice *wifi;
  Ota *ota;
  MqttClient *mqttClient;

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