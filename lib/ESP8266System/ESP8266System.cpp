#include <ESP8266System.h>

#include <ESP8266WebServer.h>

#include <DhtSensor.h>
#include <Humidifier.h>
#include <Gigrostat.h>

ESP8266System::ESP8266System(const Conf conf) {
  this->conf = conf;
  metricPrefix = conf.groupName + "_" + conf.systemName + "_" + conf.serviceName + "_";
}

String ESP8266System::getMetrics() {
  return 
    (dhtSensor  ? dhtSensor->getMetrics()  : "") +
    (humidifier ? humidifier->getMetrics() : "");
}

void ESP8266System::setup() {
  setupSerial();
  setupLED();
  setupWifi();
  setupOTA();
  setupWebServer();
}

void ESP8266System::loop() {

  if(ota) ota->loop();
  if(server) server->handleClient();
  if(gigrostat) gigrostat->loop();
  
  delay(100);
}

// PRIVATE
//============================================================================>

void ESP8266System::setupSerial() {
  Serial.begin(115200);
}

void ESP8266System::setupLED() {
  pinMode(LED_BUILTIN, OUTPUT);
  offLed();
}

void ESP8266System::setupWifi() {
  wifi = new WifiDevice(conf.ssid, conf.password, [this] (bool in) {
    Serial.print(".");
    if (in) onLed();
    else offLed();
  }); 
}

void ESP8266System::setupOTA() {
  ota = new OTA(conf.systemName, conf.serviceName);
}

void ESP8266System::setupWebServer() {

  server = new ESP8266WebServer();
  
  server->on("/metrics", [this] {
    withBlink([&] {
      server->send(200, "text/plain", getMetrics());
    });
  });

  server->begin();
  Serial.println("[HTTP] Started, port: 80");
}

void ESP8266System::setupPin(const u8 pin, const u8 mode) {
  pinMode(pin, mode);
}

void ESP8266System::setupDHT(const u8 pin, const u8 type) {
  dhtSensor = new DhtSensor(pin, type, metricPrefix);
}

void ESP8266System::setupHumidifier(const u8 pin, const u8 statePin) {
  humidifier = new Humidifier(pin, statePin, metricPrefix);
}

void ESP8266System::setupGigrostat(const real32 min, const real32 max) {
  gigrostat = new Gigrostat(dhtSensor, humidifier, metricPrefix);
  gigrostat->setup(min, max);
}

void ESP8266System::onPin(const u8 pin) {
  digitalWrite(pin, HIGH);
}

void ESP8266System::offPin(const u8 pin) {
  digitalWrite(pin, LOW);
}

void ESP8266System::onLed() {
  offPin(LED_BUILTIN);
}

void ESP8266System::offLed() {
  onPin(LED_BUILTIN);
}

void ESP8266System::withBlink(const std::function<void(void)> body) {
  onLed();
  body();
  offLed();
}