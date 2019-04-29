#include "System.h"

#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include "infras/ntp/NtpClient.h"

#include "actuators/humidifier/Humidifier.h"
#include "controls/gigrostat/Gigrostat.h"

WiFiUDP udp;

System::System(const Conf &conf) {
  this->conf = conf;
}

void System::setup() {

  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  offLed();

  wifi = new WifiDevice(conf.ssid, conf.password, [this](bool in) {
    Serial.print(".");
    in ? onLed() : offLed();
  });

  ntpClient = new NtpClient(udp);
  ntpClient->begin();
  ntpClient->update();

  Serial.println(String("[System] current UTC: ") + ntpClient->getFormattedTime());

  ota = new Ota(conf.system, conf.service);

  const String id = conf.group + "/" + conf.system + "/" + conf.service; // TODO create conf end log layer

  mqttClient = new MqttClient("su69.org", 1883, "", "", id);

  mqttClient->subscribe("commands", [&] (char* topic, u8* message, u32 length) {

    message[length] = 0;

    Serial.print("[MqttClient] topic: ");Serial.print(topic);
    Serial.print(", message:");Serial.println((char *) message);

    DynamicJsonDocument state(1024);
    deserializeJson(state, message);

    lighting->setState(state);


  });

  metricSender = new MetricSender(mqttClient, 2000, [this] {

    onLed();

    std::vector<String> metrics;

    if (dhtSensor)      metrics.push_back(dhtSensor->metrics());

    if (humidifier)     metrics.push_back(humidifier->metrics());
    if (lighting)       metrics.push_back(lighting->metrics());
    if (ventilation)    metrics.push_back(ventilation->metrics());

    offLed();

    return metrics;
  });
}

void System::loop() {

  ntpClient->update();

  if (ota)          ota->loop();
  if (mqttClient)   mqttClient->loop();
  if (metricSender) metricSender->loop();
  if (gigrostat)    gigrostat->loop();
}

void System::setupPin(u8 pin, u8 mode) {
  pinMode(pin, mode);
}

void System::setupDht(u8 pin, u8 type) {
  dhtSensor = new DhtSensor(pin, type);
}

void System::setupHumidifier(u8 pin, u8 statePin) {
  humidifier = new Humidifier(pin, statePin);
}

void System::setupLighting(u8 pin, u16 ledNum) {
  lighting = new Lighting(pin, ledNum);
}

void System::setupVentilation(u8 pin) {
  ventilation = new Ventilation(pin);
}

void System::setupGigrostat(real32 min, real32 max) {
  gigrostat = new Gigrostat(dhtSensor, humidifier, ventilation);
  gigrostat->setup(min, max);
}

void System::onPin(u8 pin) {
  digitalWrite(pin, HIGH);
}

void System::offPin(u8 pin) {
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