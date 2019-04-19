#include <Esp8266System.h>

//#include <PubSubClient.h>

Esp8266System sys({
                      "greenhouse", "mgs", "hotbed-test",
                      "INSIDIUS 2.4GHz", "scalable"
                  });

const u8 DHT_SENSOR_PIN = D1;
const u8 HUMIDIFIER_PIN = D4;
const u8 HUMIDIFIER_STATE_PIN = D5;
const u8 VENTILATION_PIN = D6;


//const char* mqttServer = "m11.cloudmqtt.com";
//const int mqttPort = 12948;
//const char* mqttUser = "YourMqttUser";
//const char* mqttPassword = "YourMqttUserPassword";



//PubSubClient *client;
//
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
//}

void setup(void) {
  sys.setup();
  sys.setupDHT(DHT_SENSOR_PIN, DHT22);
  sys.setupHumidifier(HUMIDIFIER_PIN, HUMIDIFIER_STATE_PIN);
  sys.setupVentilation(VENTILATION_PIN);
  sys.setupGigrostat(96, 99);

//  WiFiClient espClient;
//  client = new PubSubClient(espClient);
//
//  client->setServer(mqttServer, mqttPort);
//  client->setCallback(callback);
//
//  while (!client->connected()) {
//    Serial.println("Connecting to MQTT...");
//
//    if (client->connect("ESP8266Client", mqttUser, mqttPassword )) {
//
//      Serial.println("connected");
//
//    } else {
//
//      Serial.print("failed with state ");
//      Serial.print(client->state());
//      delay(2000);
//
//    }
//  }
//
//  client->publish("esp/test", "Hello from ESP8266");
//  client->subscribe("esp/test");
}

void loop(void) {
  sys.loop();
//  client->loop();
}