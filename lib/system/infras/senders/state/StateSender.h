#ifndef StateSender_h
#define StateSender_h

#include "../../mqtt/MqttClient.h"

class StateSender {

private:
  MqttClient *client;
  u16 period;
  std::function<std::vector<String>(void)> states;
  u64 lastSentTime = 0;

public:
  StateSender(MqttClient *client, u16 period,
      const std::function<std::vector<String>(void)> &states) {
    this->client = client;
    this->period = period;
    this->states = states;
  }

  void loop() {

    const long now = millis();

    if (now - lastSentTime >= period) {

      lastSentTime = now;

      if (client->enabled())
        for (auto &state : states())
          client->publish("states", state);
    }
  }
};

#endif