#ifndef MqttClient_h
#define MqttClient_h

#include <Arduino.h>
#include "PubSubClient.h"

class MqttClient {
public:
  MqttClient(
      const String &host, u16 port,
      const String &user, const String &password,
      const String &topicGroup);

  void publish(const String &topic, const String &message);
  void subscribe(const String &topic, std::function<void(char *, u8 *, u32)> onMessage);

  void loop();

  bool enabled();

private:
  String host;
  u16 port;
  String user;
  String password;

  String topicGroup;
  String topic;

  u64 lastReconnectAttempt = 0;
  bool connect(String topic);
};

#endif