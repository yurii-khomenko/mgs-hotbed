#ifndef MqttClient_h
#define MqttClient_h

#include <PubSubClient.h>

class MqttClient {
public:
  MqttClient(String host, u16 port, String user, String password);
  void loop();
private:
  String user;
  String password;
  long lastReconnectAttempt = 0;
  long latency = 250;
  bool reconnect();
//  void callback(char* topic, byte* payload, u32 length);
};

#endif