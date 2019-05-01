#ifndef System_h
#define System_h

#include <Arduino.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

#include "infras/wifi/WifiDevice.h"
#include "infras/ntp/NtpClient.h"
#include "infras/ota/Ota.h"
#include "infras/mqtt/MqttClient.h"
#include "infras/senders/metric/MetricSender.h"

#include "sensors/dht/DhtSensor.h"

#include "actuators/humidifier/Humidifier.h"
#include "actuators/ventilation/Ventilation.h"
#include "actuators/lighting/Lighting.h"

#include "controls/gigrostat/Gigrostat.h"

struct Conf {

  String group;
  String system;
  String service;

  String ssid;
  String password;
};

class System {

private:
  Conf conf;
  WiFiUDP udp;

public:
  System(const Conf &conf) {
    this->conf = conf;
  }

  WifiDevice *wifi;
  NtpClient *ntpClient;
  Ota *ota;
  MqttClient *mqttClient;
  MetricSender *metricSender;

  DhtSensor *dhtSensor;

  Humidifier *humidifier;
  Lighting *lighting;
  Ventilation *ventilation;

  Gigrostat *gigrostat;

  void setup() {

    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    wifi = new WifiDevice(conf.ssid, conf.password, [this](bool in) {
      Serial.print(".");
      digitalWrite(LED_BUILTIN, in);
    });

    ntpClient = new NtpClient(udp);
    ntpClient->begin();
    ntpClient->update();

    Serial.println(String("[System] current UTC: ") + ntpClient->getFormattedTime());

    ota = new Ota(conf.system, conf.service);

    const String id = conf.group + "/" + conf.system + "/" + conf.service; // TODO create conf end log layer

    mqttClient = new MqttClient("su69.org", 1883, "", "", id);

    mqttClient->subscribe("commands", [&](char *topic, u8 *message, u32 length) {

      message[length] = 0;

      Serial.print("[MqttClient] topic: ");
      Serial.print(topic);
      Serial.print(", message:");
      Serial.println((char *) message);

      DynamicJsonDocument state(1024);
      deserializeJson(state, message);

      if (humidifier) humidifier->setState(state);
      if (lighting) lighting->setState(state);
      if (ventilation) ventilation->setState(state);
    });

    metricSender = new MetricSender(mqttClient, 2000, [this] {

      digitalWrite(LED_BUILTIN, HIGH);

      std::vector<String> metrics;

      if (dhtSensor)      metrics.push_back(dhtSensor->getState());

      if (humidifier)     metrics.push_back(humidifier->getState());
      if (lighting)       metrics.push_back(lighting->getState());
      if (ventilation)    metrics.push_back(ventilation->getState());

      digitalWrite(LED_BUILTIN, LOW);

      return metrics;
    });
  }

  void loop() {

    ntpClient->update();

    if (ota)          ota->loop();
    if (mqttClient)   mqttClient->loop();
    if (metricSender) metricSender->loop();
    if (gigrostat)    gigrostat->loop();}

  void enableDht(u8 pin, u8 type) {
    dhtSensor = new DhtSensor(pin, type);
  }
  void disableDht() {
    delete dhtSensor;
  }

  void enableHumidifier(u8 pin, u8 pinStatus) {
    humidifier = new Humidifier(pin, pinStatus);
  }
  void disableHumidifier() {
    delete humidifier;
  }

  void enableLighting(u8 pin, u16 ledNum) {
    delete lighting;
    lighting = new Lighting(pin, ledNum);
  }
  void disableLighting() {
    delete lighting;
  }

  void enableVentilation(u8 pin) {
    ventilation = new Ventilation(pin);
  }
  void disableVentilation() {
    delete ventilation;
  }

  void enableGigrostat(real32 min, real32 max) {
    gigrostat = new Gigrostat(dhtSensor, humidifier, ventilation);
    gigrostat->setHumidity(min, max);
  }
  void disableGigrostat() {
    delete gigrostat;
  }

  void withBlink(const std::function<void(void)> &body) {
    digitalWrite(LED_BUILTIN, HIGH);
    body();
    digitalWrite(LED_BUILTIN, LOW);
  }
};

#endif