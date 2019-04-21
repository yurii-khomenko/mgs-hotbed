#ifndef MqttClient_h
#define MqttClient_h

#include "PubSubClient.h"

class MqttClient {
public:
  MqttClient(String host, u16 port, String user, String password, std::function<void(char*, u8*, u32)> onMessage);

  void publish(String topic, String message);

  void loop();

private:
  String user;
  String password;
  long lastReconnectAttempt = 0;
  bool reconnect();
};

#endif