#include <WifiDevice.h>
#include <ESP8266WiFi.h>

WifiDevice::WifiDevice(String ssid, String password, std::function<void(bool)> onProgress) {

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("[Wifi] Try to connect, ssid: " + WiFi.SSID() + " ");

  while (WiFi.status() != WL_CONNECTED) {
    if (onProgress) onProgress(true);
    delay(50);
    if (onProgress) onProgress(false);
    delay(50);
  };

  Serial.println(" connected, ip: " + WiFi.localIP().toString());
}