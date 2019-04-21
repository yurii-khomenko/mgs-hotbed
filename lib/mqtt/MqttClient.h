#ifndef MqttClient_h
#define MqttClient_h

#include "../../.piolibdeps/PubSubClient_ID89/src/PubSubClient.h"

class MqttClient {
public:
  MqttClient(String host, u16 port, String user, String password, std::function<void(char*, u8*, u32)> onMessage);
  void loop();
private:
  String user;
  String password;
  long lastReconnectAttempt = 0;
  bool reconnect();
};

#endif