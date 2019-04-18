#ifndef OTA_H
#define OTA_H

#include <Arduino.h>

class Ota {
public:
  Ota(String systemName, String serviceName);

  void loop();
};

#endif