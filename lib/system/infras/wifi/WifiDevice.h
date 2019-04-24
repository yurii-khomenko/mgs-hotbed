#ifndef WifiDevice_H
#define WifiDevice_H

#include <Arduino.h>

class WifiDevice {
public:
  WifiDevice(const String& ssid, const String& password, std::function<void(bool)> onProgress);
};

#endif