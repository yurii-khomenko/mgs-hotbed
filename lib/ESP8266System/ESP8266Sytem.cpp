#include <ESP8266System.h>

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

ESP8266WebServer *server;
DHT *dht;

ESP8266System::ESP8266System(Conf conf) {
    Serial.begin(115200);
}

void ESP8266System::_setupWifi(String ssid, String password) {

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Wait for connect to ssid: " + conf.ssid + " ");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(">");
    }

    Serial.print("\nConnected, ssid: " + WiFi.SSID() + ", ip: ");
    Serial.println(WiFi.localIP());
}

String ESP8266System::_getDhtMetrics() {

    const float t = dht->readTemperature();
    const float h = dht->readHumidity();

    if (isnan(h) || isnan(t)) {
        const String msg = "Failed to read from DHT sensor!";
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

void ESP8266System::_setupWebServer(u16 serverPort) {

    server = new ESP8266WebServer();
    
    server->on("/metrics", [this] {
        server->send(200, "text/plain", _getMetrics());
    });

    server->begin();
    Serial.println("HTTP server started, port: 80");
}

void ESP8266System::setup() {
   _setupWifi(conf.ssid, conf.password);
   _setupWebServer(conf.serverPort);
}

void ESP8266System::loop() {
    
    server->handleClient();

    Serial.println("ip: " + WiFi.localIP().toString());
    delay(100);
}

void ESP8266System::setupDHT(u8 pin, u8 type) {
    dht = new DHT(pin, type);
}