#ifndef OTA_H
#define OTA_H

#include <Arduino.h>

class OTA {
  public:
    OTA(String systemName, String serviceName, std::function<void(void)> onProgress);
    void loop();
};

#endif