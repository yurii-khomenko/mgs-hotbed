#include "Ota.h"
#include <ArduinoOTA.h>

Ota::Ota(String system, String service) {

  const String hostname = system + "-" + service;

  Serial.print("[OTA] Start server ... ");
  ArduinoOTA.setHostname(const_cast<char *> (hostname.c_str()));

  ArduinoOTA.onStart([] {
    Serial.println("\n[OTA] Start uploading");
  });

  ArduinoOTA.onProgress([&](u32 progress, u32 total) {
    Serial.printf("[OTA] %u%%\n", (progress / (total / 100)));
  });

  ArduinoOTA.onEnd([] {
    Serial.println("\n[OTA] End uploading");
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("\n[OTA] Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("[OTA] Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("[OTA] Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("[OTA] Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("[OTA] Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("[OTA] End Failed");
  });

  ArduinoOTA.begin();
  Serial.println("ready, hostname: " + hostname);
}

void Ota::loop() {
  ArduinoOTA.handle();
}