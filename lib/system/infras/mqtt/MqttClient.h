#ifndef MqttClient_h
#define MqttClient_h

#include "PubSubClient.h"

class MqttClient {
public:
  MqttClient(
      const String& host, u16 port,
      const String& user, const String& password,
      const String& queuePrefix,
      std::function<void(char*, u8*, u32)> onMessage);

  void publish(const String& topic, const String& message);

  void loop();

private:
  String host;
  u16 port;
  String user;
  String password;
  String queuePrefix;
  u64 lastReconnectAttempt = 0;
  bool connect();
};

#endif