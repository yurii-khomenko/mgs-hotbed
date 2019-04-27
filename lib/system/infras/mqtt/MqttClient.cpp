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

  randomSeed(micros());

  client.setServer(this->host.c_str(), port);
  client.setCallback(std::move(onMessage));
}

bool MqttClient::enabled() {
  return client.connected();
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

  Serial.print(String("[MqttClient] Connecting to MQTT(") + host + ":" + port + ") ... ");

  const String clientId = String("ESP8266Client-") + String(random(0xffff), HEX);

  if (!client.connect(clientId.c_str(), user.c_str(), password.c_str())) {
    Serial.println(String("fail(") + client.state() + ")");
    return false;
  }

  Serial.println("done");
  client.publish("outTopic", "hello world");

  const String topic = String(queuePrefix) + "/commands";
  Serial.print("[MqttClient] Try to subscribe to topic " + topic + "...");
  Serial.println(client.subscribe(topic.c_str()) ? "done" : "fail");

  return client.connected();
}