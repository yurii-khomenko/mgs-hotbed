#ifndef WifiDevice_H
#define WifiDevice_H

#include <Arduino.h>

class WifiDevice {
  public:
    WifiDevice(String ssid, String password, std::function<void(void)> onProgress);
};

#endif