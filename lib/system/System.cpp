#include "System.h"

#include "controls/metricSender/MetricSender.h"
#include "actuators/humidifier/Humidifier.h"
#include "controls/gigrostat/Gigrostat.h"

System::System(const Conf &conf) {
  this->conf = conf;
  prefix = conf.group + "/" + conf.system + "/" + conf.service;
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

  ota = new Ota(conf.system, conf.service);

//  delay(200);
//  client.publish("greenhouse/mgs/hotbed-test/metrics",
//                 (String("greenhouse/mgs/hotbed-test temperature=") + String(latency)).c_str());
//
//  delay(200);

//TODO NTP Server
//TODO Realise metrics
  mqttClient = new MqttClient(
      "m24.cloudmqtt.com", 14338,
      "clctfcra", "4zqsFa4wUppB",
      prefix,
      [this] (char* topic, u8* payload, u32 length) {

    Serial.print("[MqttClient] Message arrived in topic: "); Serial.print(topic);

    payload[length] = 0;
    const String message = String((char*) payload);
    Serial.println(", message:" + message);

    //TODO right handle if metrics are empty

    Serial.println(String(prefix)  + " " + dhtSensor->metrics());
//    mqttClient->publish(String(prefix) + "/metrics", String(prefix)  + " " + dhtSensor->metrics());
    mqttClient->publish(String(prefix) + "/metrics", dhtSensor->metrics());

//    Serial.println(message.toInt());
//    mqttClient.
  });

  metricSender = new MetricSender(mqttClient, 2000, [] {

  });
}

void System::loop() {

  if (ota) ota->loop();
  if (mqttClient) mqttClient->loop();
  if (gigrostat) gigrostat->loop();

  delay(100);
}

//
//============================================================================>

String System::metrics() {
  return
      (dhtSensor ? dhtSensor->metrics() : "") +
      (humidifier ? humidifier->metrics() : "") +
      (ventilation ? ventilation->metrics() : "");
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