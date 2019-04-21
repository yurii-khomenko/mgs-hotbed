#ifndef MetricSender_h
#define MetricSender_h

#include "../../infras/mqtt/MqttClient.h"

class MetricSender {
public:
  MetricSender(MqttClient *client, u16 period, std::function<void(void)> body);

  void loop();

private:

};

#endif