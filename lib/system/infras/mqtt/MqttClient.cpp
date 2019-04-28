#include "MqttClient.h"
#include <WiFiClient.h>

WiFiClient wifiClient;
PubSubClient client(wifiClient);

//TODO: integrate name conventions about group system service (greenhouse,hotbad,prod-1,sersors,dht,temperature)
MqttClient::MqttClient(
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

void MqttClient::publish(const String &topic, const String &message) {
  client.publish((topicGroup + topic).c_str(), message.c_str());
}

// TODO: upgrade to for any topics(currently only one support)
void MqttClient::subscribe(const String &topic, std::function<void(char *, u8 *, u32)> onMessage) {
  this->topic = topic;
  client.setCallback(std::move(onMessage));
}

void MqttClient::loop() {
  !client.connected() ? connect(topic) : client.loop();
}

bool MqttClient::enabled() {
  return client.connected();
}

// PRIVATE
//============================================================================>

//TODO: rename to subscribe("commands", [&] (char* topic, u8* payload, u32 length) { }) for each topic
//TODO: add queue group (set only once queue prefix)

bool MqttClient::connect(String topic) {

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

  Serial.print("[MqttClient] Try to subscribe to topic " + topic + "...");
  Serial.println(client.subscribe((topicGroup + topic).c_str()) ? "done" : "fail");

  return client.connected();
}