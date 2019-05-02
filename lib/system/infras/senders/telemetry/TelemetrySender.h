#ifndef StatusSender_h
#define StatusSender_h

#include "../../mqtt/MqttClient.h"

class TelemetrySender {

private:
  MqttClient *client;
  u16 period;
  std::function<std::vector<String>(void)> telemetries;
  u64 lastSentTime = 0;

public:
  TelemetrySender(MqttClient *client, u16 period,
      const std::function<std::vector<String>(void)> &telemetries) {
    this->client = client;
    this->period = period;
    this->telemetries = telemetries;
  }

  void loop() {

    const long now = millis();

    if (now - lastSentTime >= period) {

      lastSentTime = now;

      if (client->enabled())
        for (auto &telemetry : telemetries())
          client->publish("telemetries", telemetry);
    }
  }
};

#endif