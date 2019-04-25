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

    Serial.println("[MetricSender] Loop");

    lastSentTime = now;

    for (auto &m : metrics()) {

      Serial.print("[MetricSender] Send metric to mqtt: ");
      Serial.println(m);

      client->publish(queuePrefix + "/metrics", m);

    }
  }
}