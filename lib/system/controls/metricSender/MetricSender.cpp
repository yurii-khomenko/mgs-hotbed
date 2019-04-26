#include "MetricSender.h"

MetricSender::MetricSender(
    MqttClient *client, u16 period,
    const String& queuePrefix,
    const std::function<std::vector<String>(void)> &metrics) {

  this->client = client;
  this->period = period;
  this->queuePrefix = queuePrefix;
  this->metrics = metrics;
}

void MetricSender::loop() {

  const long now = millis();

  if(now - lastSentTime >= period) {

    lastSentTime = now;

    for (auto &m : metrics())
      client->publish(queuePrefix + "/metrics", m);
  }
}