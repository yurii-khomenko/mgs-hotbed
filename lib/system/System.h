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
#include "infras/senders/state/StateSender.h"

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
  StateSender *stateSender; //TODO: move to Task

  DhtSensor *dhtSensor;

  Humidifier *humidifier;
  Lighting *lighting;
  Ventilation *ventilation;

  Gigrostat *gigrostat;

  Scheduler scheduler;

  void setup() {
    enableSystem();
  }

  const u8 DHT_SENSOR_PIN = D1;
  const u8 HUMIDIFIER_PIN = D4;
  const u8 HUMIDIFIER_STATE_PIN = D5;
  const u8 VENTILATION_PIN = D6;
  const u8 LIGHTING_PIN = D7;

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
      withBlink ([&] {
        message[length] = 0;

        Serial.print("[MqttClient] topic: ");
        Serial.print(topic);
        Serial.println((String) ", length: " + length + ", message:");
        Serial.println((char *) message);

        DynamicJsonDocument config(length * 2);
        deserializeJson(config, message);

        configHumidifier(config); //TODO: BUG: if double disable - device reset
        configLighting(config);
        configVentilation(config);
      });
    });

    stateSender = new StateSender(mqttClient, 2000, [this] {

      digitalWrite(LED_BUILTIN, LOW);

      std::vector<String> states;

      if (dhtSensor)      states.push_back(dhtSensor->getState());

      if (humidifier)     states.push_back(humidifier->getState());
      if (lighting)       states.push_back(lighting->getState());
      if (ventilation)    states.push_back(ventilation->getState());

      digitalWrite(LED_BUILTIN, HIGH);

      return states;
    });

    scheduler.init();
    scheduler.startNow();
  }

  void configHumidifier(const DynamicJsonDocument &config) {
    if (!config["actuators"]["humidifier"].isNull()) {
      enableHumidifier(HUMIDIFIER_PIN, HUMIDIFIER_STATE_PIN);
      humidifier->setConfig(config);
    } else
      delete humidifier;
  }

  void configLighting(const DynamicJsonDocument &config) {
    if (!config["actuators"]["lighting"].isNull()) {
      enableLighting(LIGHTING_PIN, 150);
      lighting->setConfig(config);
    } else
      delete lighting;
  }

  void configVentilation(const DynamicJsonDocument &config) {
    if (!config["actuators"]["ventilation"].isNull()) {
      enableVentilation(VENTILATION_PIN);
      ventilation->setConfig(config);
    } else
      delete ventilation;
  }

  void enableDht(u8 pin, u8 type) {
    disableDht();
    dhtSensor = new DhtSensor(pin, type);
  }
  void disableDht() {
    delete dhtSensor;
  }

  void enableHumidifier(u8 pin, u8 pinState) {
    disableHumidifier();
    humidifier = new Humidifier(pin, pinState);
  }
  void disableHumidifier() {
    delete humidifier;
  }

  void enableLighting(u8 pin, u16 ledNum) {
    delete lighting;
    lighting = new Lighting(pin, ledNum);
  }

  void enableVentilation(u8 pin) {
    delete ventilation;
    ventilation = new Ventilation(pin);
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

    if (stateSender)  stateSender->loop();

    scheduler.execute();
  }
};

#endif