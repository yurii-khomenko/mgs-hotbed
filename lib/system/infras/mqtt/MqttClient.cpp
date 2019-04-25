#include "MqttClient.h"
#include <WiFiClient.h>

WiFiClient wifiClient;
PubSubClient client(wifiClient);

MqttClient::MqttClient(
    const String& host, u16 port,
    const String& user, const String& password,
    const String& queuePrefix,
    std::function<void(char*, u8*, u32)> onMessage) {

  this->host = host;
  this->port = port;
  this->user = user;
  this->password = password;
  this->queuePrefix = queuePrefix;

  client.setServer(host.c_str(), port);
  client.setCallback(std::move(onMessage));

  while (!client.connected()) {

//    Serial.println("[MqttClient] Connecting to MQTT...");

    if (connect())
      Serial.println("[MqttClient] Connected");
    else {
      Serial.print("[MqttClient] failed with state ");
      Serial.print(client.state());
      delay(500);
    }
  }
}

void MqttClient::publish(const String& topic, const String& message) {
  client.publish(topic.c_str(), message.c_str());
}

void MqttClient::loop() {






  !client.connected() ? connect() : client.loop();


  if (!client.connected()) {
    const long now = millis();
    if (now - lastReconnectAttempt > 2000) {
      lastReconnectAttempt = now;
      if (connect())
        lastReconnectAttempt = 0;
    }
  } else
    client.loop();
}

// PRIVATE
//============================================================================>

bool MqttClient::connect() {

  Serial.print("[MqttClient] Connecting to MQTT(" + host + ":" + port + ")...");

  if (client.connect("ESP8266Client", user.c_str(), password.c_str())) {
    Serial.println("done");
  }
    client.subscribe((String(queuePrefix) + "/commands").c_str());

  return client.connected();
}