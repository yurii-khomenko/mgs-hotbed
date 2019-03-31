#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const String groupName = "greenhouse";
const String systemName = "mgs";
const String serviceName = "hotbed";

const String serverHostName = systemName + "-" + serviceName;
const int serverPort = 80;

const String ssid = "INSIDIUS 2.4GHz";
const String password = "scalable";

ESP8266WebServer server(serverPort);

/*===========================================================================
| HTTP Handlers
=============================================================================*/
String getMetrics()
{
  // return getDhtMetrics();
  return "!!";
}

// String getDhtMetrics()
// {

//   float t = dht.readTemperature();
//   float h = dht.readHumidity();

//   if (isnan(h) || isnan(t))
//   {
//     String msg = "Failed to read from DHT sensor!";
//     Serial.println(msg);
//     return msg;
//   }

//   float hic = dht.computeHeatIndex(t, h, false);

//   return String("group_system_service_temperature ") + t + "\n" +
//          "group_system_service_heatindex " + hic + "\n" +
//          "group_system_service_humidity " + h + "\n";
// }

void setupWifi(String ssid, String password)
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(">");
  }

  Serial.print("\nConnected, ssid: " + WiFi.SSID() + ", ip: ");
  Serial.println(WiFi.localIP());
}

void setupMDNS(String hostname)
{
  if (!MDNS.begin(hostname))
  {
    Serial.println("Error setting up MDNS responder!");
    while (1)
    {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started, hostname: " + hostname + ".local");
}

void setupWebServer()
{
  server.on("/metrics", []() {
    server.send(200, "text/plain", getMetrics());
  });

  server.begin();
  Serial.println("HTTP server started, port: " + String(serverPort));
}

/*===========================================================================
| MAIN
=============================================================================*/
void setup(void)
{
  Serial.begin(115200);

  setupWifi(ssid, password);
  setupMDNS(serverHostName);
  setupWebServer();
}

void loop(void)
{
  server.handleClient();
  MDNS.update();
}

// // #include "DHT.h"

// //#include "DHT.h"
// //#define DHTPIN 2
// //#define DHTTYPE DHT22
// // DHT dht(16, DHT22);

// /*===========================================================================
//   | MAIN
//   =============================================================================*/

// void setup(void)
// {
//   Serial.begin(115200);

//   setupWifi(ssid, password, hostName);
//   setupWebServer();

//   // Add service to MDNS-SD
//   MDNS.addService("http", "tcp", 80);

//   //dht.begin();
// }

// void loop(void)
// {
//   server.handleClient();
//   MDNS.update();
//   // Serial.print("IP address: ");
//   // Serial.println(WiFi.localIP());

//   // delay(100);
// }