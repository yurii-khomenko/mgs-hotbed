#ifndef System_h
#define System_h

#include <Arduino.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <TaskScheduler.h>

#include "infras/wifi/WifiDevice.h"
#include "infras/ntp/NtpClient.h"
#include "infras/ota/Ota.h"
#include "infras/mqtt/MqttClient.h"
#include "infras/senders/telemetry/TelemetrySender.h"

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
public:
  System(const Conf &conf) {
    this->conf = conf;
  }

  Conf conf;
  WiFiUDP udp;

  WifiDevice *wifi;
  NtpClient *ntpClient;
  Ota *ota;
  MqttClient *mqttClient;
  TelemetrySender *telemetrySender; //TODO: move to Task

  DhtSensor *dhtSensor;

  Humidifier *humidifier;
  Lighting *lighting;
  Ventilation *ventilation;

  Gigrostat *gigrostat;

  Scheduler scheduler;

  void enableSystem() {

    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    wifi = new WifiDevice(conf.ssid, conf.password, [this](bool in) {
      Serial.print(".");
      digitalWrite(LED_BUILTIN, !in);
    });

    ntpClient = new NtpClient(udp);
    ntpClient->begin();
    ntpClient->update();

    Serial.println(String("[System] current UTC: ") + ntpClient->getFormattedTime());

    ota = new Ota(conf.system, conf.service);

    const String id = conf.group + "/" + conf.system + "/" + conf.service; // TODO create conf end log layer

    mqttClient = new MqttClient("su69.org", 1883, "", "", id);

    mqttClient->subscribe("configs", [&](char *topic, u8 *message, u32 length) {

      message[length] = 0;

      Serial.print("[MqttClient] topic: ");
      Serial.print(topic);
      Serial.print(", message:");
      Serial.println((char *) message);

      DynamicJsonDocument config(1024); //TODO: use length field
      deserializeJson(config, message);

      if (humidifier)   humidifier->setConfig(config);
      if (lighting)     lighting->setConfig(config);
      if (ventilation)  ventilation->setConfig(config);
    });

    telemetrySender = new TelemetrySender(mqttClient, 2000, [this] {

      digitalWrite(LED_BUILTIN, LOW);

      std::vector<String> telemetries;

      if (dhtSensor)      telemetries.push_back(dhtSensor->getTelemetry());

      if (humidifier)     telemetries.push_back(humidifier->getTelemetry());
      if (lighting)       telemetries.push_back(lighting->getTelemetry());
      if (ventilation)    telemetries.push_back(ventilation->getTelemetry());

      digitalWrite(LED_BUILTIN, HIGH);

      return telemetries;
    });

    scheduler.init();
    scheduler.startNow();
  }

  void enableDht(u8 pin, u8 type) {
    delete dhtSensor;
    dhtSensor = new DhtSensor(pin, type);
  }
  void disableDht() {
    delete dhtSensor;
  }

  void enableHumidifier(u8 pin, u8 pinState) {
    delete humidifier;
    humidifier = new Humidifier(pin, pinState);
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
    delete ventilation;
    ventilation = new Ventilation(pin);
  }
  void disableVentilation() {
    delete ventilation;
  }

  void enableGigrostat(real32 min, real32 max) {
    delete gigrostat;
    gigrostat = new Gigrostat(dhtSensor, humidifier, ventilation);
    gigrostat->setHumidity(min, max);
  }
  void disableGigrostat() {
    delete gigrostat;
  }

  void withBlink(const std::function<void(void)> &body) {
    digitalWrite(LED_BUILTIN, LOW);
    body();
    digitalWrite(LED_BUILTIN, HIGH);
  }

  void loop() {

    ntpClient->update();

    if (ota)          ota->loop();
    if (mqttClient)   mqttClient->loop();
    if (gigrostat)    gigrostat->loop();

    if (telemetrySender) telemetrySender->loop();

    scheduler.execute();
  }
};

#endif