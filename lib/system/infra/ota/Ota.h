#ifndef Ota_h
#define Ota_h

#include <Arduino.h>

class Ota {
public:
  Ota(String systemName, String serviceName);

  void loop();
};

#endif