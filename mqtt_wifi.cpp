#include "mqtt_wifi.h"
#include <string>

WiFiClient espClient;
PubSubClient* client = new PubSubClient(espClient);
long lastMsg = 0;
int value = 0;


void setup_wifi() {
  // This setup_wifi() was adapted from Rui Santos at RandomNerdTutorials. Thank you Rui! https://randomnerdtutorials.com/esp32-mqtt-publish-subscribe-arduino-ide/
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to Wifi SSID ");
  Serial.println(mySSID);

  WiFi.begin(mySSID, myPassword);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup_mqtt() {
  Serial.print("Connecting to MQTT Broker at ");
  Serial.println(my_mqtt_server);
  client->setServer(my_mqtt_server, 1883);
  client->setCallback(mqtt_callback);
  mqtt_connect();
} // setup_mqtt()

bool mqtt_connect() {
  while (!client->connected()) {
    if (client->connect("ESP32", my_mqtt_id, my_mqtt_pwd)) {
      Serial.println("Connected to MQTT");
      return true;
    } 
    else {
      Serial.println("Connection attempt to MQTT failed.");
      Serial.println(client->state());
      delay(5000);
    }
  }
  return true;  
} // mqtt_connect()

// This callback was inspired by the void callback() by Rui Santos at https://randomnerdtutorials.com/esp8266-and-node-red-with-mqtt/
void mqtt_callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message: ");
  String messageTemp;
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  if (topic=="greenhouse/lights") {
    if (messageTemp=="on") {
      digitalWrite(LED_PIN, HIGH);
    }
    else if (messageTemp=="off") {
      digitalWrite(LED_PIN, LOW);
    }
  }
}


////////////////////////


