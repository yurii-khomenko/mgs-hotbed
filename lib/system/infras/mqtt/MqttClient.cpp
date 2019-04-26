#include "MqttClient.h"
#include <WiFiClient.h>

WiFiClient wifiClient;
PubSubClient client(wifiClient);

MqttClient::MqttClient(
    const String &host, u16 port,
    const String &user, const String &password,
    const String &queuePrefix,
    std::function<void(char *, u8 *, u32)> onMessage) {

  this->host = host;
  this->port = port;
  this->user = user;
  this->password = password;
  this->queuePrefix = queuePrefix;

  client.setServer(host.c_str(), port);
  client.setCallback(std::move(onMessage));

  while (!client.connected()) {
    connect();
    delay(250);
  }
}

void MqttClient::publish(const String &topic, const String &message) {
  client.publish(topic.c_str(), message.c_str());
}

void MqttClient::loop() {
  !client.connected() ? connect() : client.loop();
}

// PRIVATE
//============================================================================>

bool MqttClient::connect() {

  const long now = millis();
  if (now - lastReconnectAttempt < 2000) return false;

  lastReconnectAttempt = now;

  Serial.print("[MqttClient] Connecting to MQTT(" + host + ":" + port + ") ... ");

  if (!client.connect("ESP8266Client", user.c_str(), password.c_str())) {
    Serial.println(String("fail(") + client.state() + ")");
    return false;
  }

  Serial.println("done");

  const String topic = String(queuePrefix) + "/commands";
  Serial.print("[MqttClient] Try to subscribe to topic " + topic + " ... ");
  Serial.println(client.subscribe(topic.c_str()) ? "done" : "fail");

  return client.connected();
}