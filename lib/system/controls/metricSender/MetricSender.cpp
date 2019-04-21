#include "MetricSender.h"

MetricSender::MetricSender(MqttClient *client, u16 period, std::function<void(void)> body) {
  this->client = client;
  this->period = period;
  this->body = body;
}

void MetricSender::loop() {

//    long now = millis();
//    if (now - lastReconnectAttempt > 2000) {
//      lastReconnectAttempt = now;
//      if (reconnect())
//        lastReconnectAttempt = 0;
//    }
//  } else
//    client.loop();
}

