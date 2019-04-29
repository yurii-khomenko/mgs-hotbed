#include "MetricSender.h"

MetricSender::MetricSender(
    MqttClient *client, u16 period,
    const std::function<std::vector<String>(void)> &metrics) {

  this->client = client;
  this->period = period;
  this->metrics = metrics;
}

void MetricSender::loop() {

  const long now = millis();

  if (now - lastSentTime >= period) {

    lastSentTime = now;

    if (client->enabled())
      for (auto &m : metrics())
        client->publish("metrics", m);
  }
}