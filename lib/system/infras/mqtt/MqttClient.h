#ifndef MqttClient_h
#define MqttClient_h

#include <Arduino.h>
#include <WiFiClient.h>
#include "PubSubClient.h"

WiFiClient wifiClient;
PubSubClient client(wifiClient);

class MqttClient {
public:
  MqttClient(
      const String &host, u16 port,
      const String &user, const String &password,
      const String &topicGroup) {

    this->host = host;
    this->port = port;
    this->user = user;
    this->password = password;

    this->topicGroup = topicGroup + "/";

    randomSeed(micros());

    client.setServer(this->host.c_str(), port);
  }

  void publish(const String &topic, const String &message) {
    client.publish((topicGroup + topic).c_str(), message.c_str());
  }
  // TODO: upgrade to for any topics(currently only one support)
  void subscribe(const String &topic, std::function<void(char *, u8 *, u32)> onMessage) {
    this->topic = topic;
    client.setCallback(std::move(onMessage));
  }

  bool enabled() {
    return client.connected();
  }

  void loop() {
    !client.connected() ? connect(topic) : client.loop();
  }

private:
  String host;
  u16 port;
  String user;
  String password;

  String topicGroup;
  String topic;

  u64 lastReconnectAttempt = 0;
  //TODO: rename to subscribe("commands", [&] (char* topic, u8* payload, u32 length) { }) for each topic
//TODO: add queue group (set only once queue prefix)

  bool connect(const String &topic) {

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

    Serial.print("[MqttClient] Try to subscribe to topic '" + topic + "'...");
    Serial.println(client.subscribe((topicGroup + topic).c_str()) ? "done" : "fail");

    return client.connected();
  }
};

#endif