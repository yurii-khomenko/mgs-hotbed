#include <Esp8266System.h>

#include <DhtSensor.h>
#include <Humidifier.h>
#include <Gigrostat.h>

Esp8266System::Esp8266System(const Conf &conf) {
  this->conf = conf;
  metricPrefix = conf.groupName + "_" + conf.systemName + "_" + conf.serviceName + "_";
}

String Esp8266System::metrics() {
  return
      (dhtSensor ? dhtSensor->metrics() : "") +
      (humidifier ? humidifier->metrics() : "") +
      (ventilation ? ventilation->metrics() : "");
}

void Esp8266System::setup() {

  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  offLed();

  wifi = new WifiDevice(conf.ssid, conf.password, [this](bool in) {
    Serial.print(".");
    if (in) onLed();
    else offLed();
  });

  ota = new Ota(conf.systemName, conf.serviceName);

  server = new ESP8266WebServer();
  server->on("/metrics", [this] {
    withBlink([&] {
      server->send(200, "text/plain", metrics());
    });
  });
  server->begin();
  Serial.println("[HTTP] Started, port: 80");
}

void Esp8266System::loop() {

  if (ota) ota->loop();
  if (server) server->handleClient();
  if (gigrostat) gigrostat->loop();

  delay(100);
}

// PRIVATE
//============================================================================>

void Esp8266System::setupPin(const u8 pin, const u8 mode) {
  pinMode(pin, mode);
}

void Esp8266System::setupDHT(const u8 pin, const u8 type) {
  dhtSensor = new DhtSensor(pin, type, metricPrefix);
}

void Esp8266System::setupHumidifier(const u8 pin, const u8 statePin) {
  humidifier = new Humidifier(pin, statePin, metricPrefix);
}

void Esp8266System::setupVentilation(const u8 pin) {
  ventilation = new Ventilation(pin, metricPrefix);
}

void Esp8266System::setupGigrostat(const real32 min, const real32 max) {
  gigrostat = new Gigrostat(dhtSensor, humidifier, ventilation);
  gigrostat->setup(min, max);
}

void Esp8266System::onPin(const u8 pin) {
  digitalWrite(pin, HIGH);
}

void Esp8266System::offPin(const u8 pin) {
  digitalWrite(pin, LOW);
}

void Esp8266System::onLed() {
  offPin(LED_BUILTIN);
}

void Esp8266System::offLed() {
  onPin(LED_BUILTIN);
}

void Esp8266System::withBlink(const std::function<void(void)> &body) {
  onLed();
  body();
  offLed();
}