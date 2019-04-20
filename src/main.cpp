#include <Esp8266System.h>
#include <PubSubClient.h>

Esp8266System sys({
                      "greenhouse", "mgs", "hotbed-test",
                      "INSIDIUS 2.4GHz", "scalable"
                  });

const u8 DHT_SENSOR_PIN = D1;
const u8 HUMIDIFIER_PIN = D4;
const u8 HUMIDIFIER_STATE_PIN = D5;
const u8 VENTILATION_PIN = D6;


//const char* mqttServer = "m24.cloudmqtt.com";
//const int mqttPort = 14338;
//const char* mqttUser = "clctfcra";
//const char* mqttPassword = "4zqsFa4wUppB";
//long lastReconnectAttempt = 0;

//int latency = 250;

//WiFiClient espClient;
//PubSubClient client(espClient);

//void callback(char* topic, byte* payload, u32 length) {
//
//  Serial.print("Message arrived in topic: ");
//  Serial.println(topic);
//
//  Serial.print("Message:");
//  for (u32 i = 0; i < length; i++)
//    Serial.print((char)payload[i]);
//
//  Serial.println();
//  Serial.println("-----------------------");
//
//  latency = (payload[0] - 48) * 100;
//
//  Serial.print("set latency to: ");
//  Serial.println(latency);
//}



//boolean reconnect() {
//  if (client.connect(mqttUser))
//    client.subscribe("greenhouse/mgs/hotbed-test/commands");
//  return client.connected();
//}

void setup(void) {
  sys.setup();
  sys.setupDHT(DHT_SENSOR_PIN, DHT22);
//  sys.setupHumidifier(HUMIDIFIER_PIN, HUMIDIFIER_STATE_PIN);
//  sys.setupVentilation(VENTILATION_PIN);
//  sys.setupGigrostat(96, 99);

}

void loop(void) {
  sys.loop();

//  if (!client.connected()) {
//    long now = millis();
//    if (now - lastReconnectAttempt > 5000) {
//      lastReconnectAttempt = now;
//      // Attempt to reconnect
//      if (reconnect()) {
//        lastReconnectAttempt = 0;
//      }
//    }
//  } else {
//    // Client connected
//
//    client.loop();
//  }
//
//  sys.onLed();
//  delay(latency);
//  sys.offLed();
//  client.publish("greenhouse/mgs/hotbed-test/metrics", (String("greenhouse/mgs/hotbed-test temperature=") + String(latency)).c_str());
//
////  client.publish("greenhouse/mgs/hotbed-test/logs", String(latency).c_str());
//  delay(latency);
}