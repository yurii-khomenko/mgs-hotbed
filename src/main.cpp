#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

const String ssid = "INSIDIUS 2.4GHz";
const String password = "scalable";

const String groupName = "greenhouse";
const String systemName = "mgs";
const String serviceName = "hotbed";

const String serverHostName = systemName + "-" + serviceName;
const int serverPort = 80;

const String metricPrefix = groupName + "_" + systemName + "_" + serviceName + "_";

ESP8266WebServer server(serverPort);
DHT dht(16, DHT22);

/*===========================================================================
| HTTP Handlers
=============================================================================*/
String getDhtMetrics()
{

  const float t = dht.readTemperature();
  const float h = dht.readHumidity();

  if (isnan(h) || isnan(t))
  {
    const String msg = "Failed to read from DHT sensor!";
    Serial.println(msg);
    return msg;
  }

  const float hic = dht.computeHeatIndex(t, h, false);

  return String(metricPrefix + "temperature ") + t + "\n" +
         metricPrefix + "heatindex " + hic + "\n" +
         metricPrefix + "humidity " + h + "\n";
}

String getMetrics()
{
  return getDhtMetrics();
}

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