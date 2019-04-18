#ifndef OTA_H
#define OTA_H

#include <Arduino.h>

class OTA {
public:
  OTA(String systemName, String serviceName);

  void loop();
};

#endif