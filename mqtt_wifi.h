#ifndef MQTT_WIFI
#define MQTT_WIFI

#include "authentication.h"
#include "WiFi.h"
#include <PubSubClient.h>
#include "lights.h"


#define MSG_LENGTH 100


extern PubSubClient* client;
extern long lastMsg;
extern char topicBuffer[MSG_LENGTH];
extern char payloadBuffer[MSG_LENGTH];
extern int value;

void setup_wifi();
void setup_mqtt();

// MQTT communication
void publish(char* peripheral, char* topic, char* payload);
bool mqtt_connect();
void mqtt_callback(String topic, byte* message, unsigned int length);

#endif // end of include guard