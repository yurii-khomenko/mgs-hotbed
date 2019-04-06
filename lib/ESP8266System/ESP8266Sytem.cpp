#include <ESP8266System.h>

#include <Arduino.h>

#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

#include <ESP8266WebServer.h>
#include <DHT.h>

ESP8266WebServer *server;
DHT *dht;

ESP8266System::ESP8266System(Conf conf) {
  Serial.begin(115200);
}

void ESP8266System::_setupLED() {
  pinMode(LED_BUILTIN, OUTPUT);
  ledOff();
}

void ESP8266System::_setupWifi(String ssid, String password) {

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("[Wifi] Try to connect, ssid: " + WiFi.SSID() + " ");

  ledOn();

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(">");
    delay(250);
  }
  
  ledOff();

  Serial.println("\n[Wifi] Connected, ip: " + WiFi.localIP().toString());
}

void ESP8266System::_setupOTA() {

  const String hostname = String(conf.groupName) + "-" + conf.systemName;

  Serial.println("[OTA] Start server");
  ArduinoOTA.setHostname(const_cast<char*> (hostname.c_str()));
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([] {
    Serial.println("\n[OTA] Start uploading");
  });

  ArduinoOTA.onEnd([] {
    Serial.println("\n[OTA] End uploading");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("[OTA] Upload firmware: %u%%\n", (progress / (total / 100)));
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

String ESP8266System::_getDhtMetrics() {

  const float t = dht->readTemperature();
  const float h = dht->readHumidity();

  if (isnan(h) || isnan(t)) {
    const String msg = "[DHT] Failed to read from DHT sensor!";
    Serial.println(msg);
    return msg;
  }

  const float hic = dht->computeHeatIndex(t, h, false);

  const String metricPrefix = conf.groupName + "_" + conf.systemName + "_" + conf.serviceName + "_";

  return String(metricPrefix + "temperature ") + t + "\n" +
          metricPrefix + "heatindex " + hic + "\n" +
          metricPrefix + "humidity " + h + "\n";
}

String ESP8266System::_getMetrics() {
  return dht ? _getDhtMetrics() : ""; 
}

void ESP8266System::_setupWebServer() {

  server = new ESP8266WebServer();
  
  server->on("/metrics", [this] {
    ledOn();
    server->send(200, "text/plain", _getMetrics());
    ledOff();
  });

  server->begin();
  Serial.println("[HTTP] Started, port: 80");
}

void ESP8266System::setupDHT(u8 pin, u8 type) {
  dht = new DHT(pin, type);
}

void ESP8266System::ledOn() {
  digitalWrite(LED_BUILTIN, LOW);
}

void ESP8266System::ledOff() {
  digitalWrite(LED_BUILTIN, HIGH);
}

void ESP8266System::setup() {
  _setupLED();
  _setupWifi(conf.ssid, conf.password);
  _setupOTA();
  _setupWebServer();
}

void ESP8266System::loop() {
  server->handleClient();
  ArduinoOTA.handle();
}