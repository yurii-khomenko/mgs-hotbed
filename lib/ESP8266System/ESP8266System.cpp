#include <ESP8266System.h>

#include <Arduino.h>

#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

#include <ESP8266WebServer.h>
#include <DhtSensor.h>
#include <Humidifier.h>
#include <Gigrostat.h>

ESP8266System::ESP8266System(const Conf conf) {
  this->conf = conf;
  this->metricPrefix = conf.groupName + "_" + conf.systemName + "_" + conf.serviceName + "_";
  Serial.begin(115200);
}

void ESP8266System::setupLED() {
  pinMode(LED_BUILTIN, OUTPUT);
  offLed();
}

void ESP8266System::setupWifi() {

  WiFi.mode(WIFI_STA);
  WiFi.begin(conf.ssid, conf.password);
  Serial.print("[Wifi] Try to connect, ssid: " + WiFi.SSID() + " ");

  withBlink([] {
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(">");
      delay(100);
    }
  });

  Serial.println("\n[Wifi] Connected, ip: " + WiFi.localIP().toString());
}

void ESP8266System::setupOTA() {

  const String hostname = conf.systemName + "-" + conf.serviceName;

  Serial.println("[OTA] Start server");
  ArduinoOTA.setHostname(const_cast<char*> (hostname.c_str()));

  ArduinoOTA.onStart([] {
    Serial.println("\n[OTA] Start uploading");
  });
  
  ArduinoOTA.onProgress([this](u32 progress, u32 total) {
    withBlink([&] {
      Serial.printf("[OTA] Upload firmware: %u%%\n", (progress / (total / 100)));
    });
  });

  ArduinoOTA.onEnd([] {
    Serial.println("\n[OTA] End uploading");
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("\n[OTA] Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("[OTA] Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("[OTA] Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("[OTA] Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("[OTA] Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("[OTA] End Failed");
  });
  
  ArduinoOTA.begin();
  Serial.println("[OTA] Ready, hostname: " + hostname);
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

String ESP8266System::getMetrics() {
  return 
    (dhtSensor  ? dhtSensor->getMetrics()  : "") +
    (humidifier ? humidifier->getMetrics() : "");
}

void ESP8266System::setup() {
  setupLED();
  setupWifi();
  setupOTA();
  setupWebServer();
}

void ESP8266System::loop() {
  ArduinoOTA.handle();
  server->handleClient();
  if(gigrostat) gigrostat->loop();
  delay(100);
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