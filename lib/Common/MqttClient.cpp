#include <WiFiClient.h>
#include "MqttClient.h"

WiFiClient wifiClient;
PubSubClient client(wifiClient);

MqttClient::MqttClient(String host, u16 port, String user, String password) {

  this->user = user;
  this->user = password;

  client.setServer(host.c_str(), port);

  client.setCallback([this] (char* topic, byte* payload, u32 length) {

    Serial.print("[MqttClient] Message arrived in topic: ");
    Serial.println(topic);

    Serial.print("[MqttClient] Message:");
    for (u32 i = 0; i < length; i++)
      Serial.print((char)payload[i]);

    Serial.println();
    Serial.println("-----------------------");

    latency = (payload[0] - 48) * 100;

    Serial.print("[MqttClient] set latency to: ");
    Serial.println(latency);
  });

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

  delay(200);
  client.publish("greenhouse/mgs/hotbed-test/metrics",
                 (String("greenhouse/mgs/hotbed-test temperature=") + String(latency)).c_str());

  delay(200);
}

// PRIVATE
//============================================================================>

bool MqttClient::reconnect() {
  if (client.connect("ESP8266Client", user.c_str(), password.c_str()))
    client.subscribe("greenhouse/mgs/hotbed-test/commands");
  return client.connected();
}