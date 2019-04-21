#include "MqttClient.h"
#include <WiFiClient.h>

WiFiClient wifiClient;
PubSubClient client(wifiClient);

MqttClient::MqttClient(
    String host, u16 port,
    String user, String password,
    std::function<void(char*, u8*, u32)> onMessage) {

  this->user = user;
  this->user = password;

  client.setServer(host.c_str(), port);
  client.setCallback(std::move(onMessage));

  while (!client.connected()) {

    Serial.println("[MqttClient] Connecting to MQTT...");

    if (client.connect("ESP8266Client", user.c_str(), password.c_str()))
      Serial.println("[MqttClient] Connected");
    else {
      Serial.print("[MqttClient] failed with state ");
      Serial.print(client.state());
      delay(500);
    }
  }

//  client.publish("greenhouse/mgs/hotbed-test/logs", "Start ");

  client.subscribe("greenhouse/mgs/hotbed-test/commands");
}

void MqttClient::publish(String topic, String message) {
  client.publish(topic.c_str(), message.c_str());
}

void MqttClient::loop() {

  if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 2000) {
      lastReconnectAttempt = now;
      if (reconnect())
        lastReconnectAttempt = 0;
    }
  } else
    client.loop();
}

// PRIVATE
//============================================================================>

bool MqttClient::reconnect() {

  if (!client.connected() && client.connect("ESP8266Client", user.c_str(), password.c_str()))
    client.subscribe("greenhouse/mgs/hotbed-test/commands");

  return client.connected();
}