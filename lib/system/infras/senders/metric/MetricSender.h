#ifndef MetricSender_h
#define MetricSender_h

#include "infras/mqtt/MqttClient.h"

class MetricSender {
public:
  MetricSender(MqttClient *client, u16 period, const std::function<std::vector<String>(void)> &metrics);

  void loop();

private:
  MqttClient *client;
  u16 period;
  std::function<std::vector<String>(void)> metrics;
  u64 lastSentTime = 0;
};

#endif