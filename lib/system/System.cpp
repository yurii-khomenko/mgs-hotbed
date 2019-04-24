#include "System.h"

#include <WiFiUdp.h>
#include <NTPClient.h>

#include "controls/metricSender/MetricSender.h"
#include "actuators/humidifier/Humidifier.h"
#include "controls/gigrostat/Gigrostat.h"


const long utcOffsetInSeconds = 3600;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

System::System(const Conf &conf) {
  this->conf = conf;
}

void System::setup() {

  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  offLed();

  wifi = new WifiDevice(conf.ssid, conf.password, [this](bool in) {
    Serial.print(".");
    if (in) onLed();
    else offLed();
  });

  timeClient.begin();

//  WiFiUDP udp;
//  ntpClient = new NtpClient(udp);
//  ntpClient->begin();

  ota = new Ota(conf.system, conf.service);

  const String queuePrefix = conf.group + "/" + conf.system + "/" + conf.service;

  mqttClient = new MqttClient(
      "m24.cloudmqtt.com", 14338, "clctfcra", "4zqsFa4wUppB",
      queuePrefix,
      [&] (char* topic, u8* payload, u32 length) {

    Serial.print("[MqttClient] Message arrived in topic: "); Serial.print(topic);

    payload[length] = 0;
    const String message = String((char*) payload);
    Serial.println(", message:" + message);
  });

  metricSender = new MetricSender(mqttClient, 2000, queuePrefix, [this] {

    std::vector<String> metrics;

    if (dhtSensor) metrics.push_back(dhtSensor->metrics());
    if (humidifier) metrics.push_back(humidifier->metrics());
    if (ventilation) metrics.push_back(ventilation->metrics());

    return metrics;
  });
}

void System::loop() {

//  if (ntpClient)    ntpClient->update();
  if (ota)          ota->loop();
  if (mqttClient)   mqttClient->loop();
  if (metricSender) metricSender->loop();
  if (gigrostat)    gigrostat->loop();


  timeClient.update();
  Serial.println(timeClient.getFormattedTime());

  delay(100);
}

void System::setupPin(const u8 pin, const u8 mode) {
  pinMode(pin, mode);
}

void System::setupDHT(const u8 pin, const u8 type) {
  dhtSensor = new DhtSensor(pin, type);
}

void System::setupHumidifier(const u8 pin, const u8 statePin) {
  humidifier = new Humidifier(pin, statePin);
}

void System::setupVentilation(const u8 pin) {
  ventilation = new Ventilation(pin);
}

void System::setupGigrostat(const real32 min, const real32 max) {
  gigrostat = new Gigrostat(dhtSensor, humidifier, ventilation);
  gigrostat->setup(min, max);
}

void System::onPin(const u8 pin) {
  digitalWrite(pin, HIGH);
}

void System::offPin(const u8 pin) {
  digitalWrite(pin, LOW);
}

void System::onLed() {
  offPin(LED_BUILTIN);
}

void System::offLed() {
  onPin(LED_BUILTIN);
}

void System::withBlink(const std::function<void(void)> &body) {
  onLed();
  body();
  offLed();
}