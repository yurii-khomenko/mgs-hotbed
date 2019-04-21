#include "MetricSender.h"

MetricSender::MetricSender(MqttClient *client, u16 period, std::function<void(void)> body) {

}


void MetricSender::loop() {
  delay(500);
}

