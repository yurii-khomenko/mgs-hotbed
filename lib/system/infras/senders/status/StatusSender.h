#ifndef StatusSender_h
#define StatusSender_h

#include "../../mqtt/MqttClient.h"

class StatusSender {

private:
  MqttClient *client;
  u16 period;
  std::function<std::vector<String>(void)> statuses;
  u64 lastSentTime = 0;

public:
  StatusSender(MqttClient *client, u16 period,
      const std::function<std::vector<String>(void)> &statuses) {
    this->client = client;
    this->period = period;
    this->statuses = statuses;
  }

  void loop() {

    const long now = millis();

    if (now - lastSentTime >= period) {

      lastSentTime = now;

      if (client->enabled())
        for (auto &status : statuses())
          client->publish("statuses", status);
    }
  }
};

#endif