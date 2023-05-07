#include "espClientBLE.h"
#include <arduino-timer.h>
#include "mqtt_wifi.h"
#include "lights.h"
#define TIME_INTERVAL 3000 // milliseconds for timer

// Initialize BLE house keeping flags
boolean doConnect = false;
boolean connected = false;
boolean doScan = false;

// House keepting MQTT
boolean subscribed = false;

Timer<10> timer;

void setup() {
  Serial.begin(115200);
  setup_wifi();
  setup_lights();

  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 30 seconds.
  pScanResults = scanForBLEserver(30);
  //Timer setup
  timer.every(TIME_INTERVAL, manageConnection); // manages BLE house keeping flags and scans for, connects and reconnects to BLE server 
  timer.every(TIME_INTERVAL, printReadings); // print BLE characteristics (with sensor values) to console 

  Serial.print("Scan Results: "); Serial.println(pScanResults.getCount());

  setup_mqtt();
  subscribed = client->subscribe("greenhouse/lights");
  delay(1000);
  Serial.print("Subscribed: ");
  Serial.println(subscribed);
  DPRINTLN("/////////// Setup finished. ////////////");
} // End of setup.


// This is the Arduino main loop function.
void loop() {
  while (!client->connected()) {
    subscribed = false;
    Serial.println("Lost connection to MQTT broker, trying to reconnect.");
    mqtt_connect();
    subscribed = client->subscribe("greenhouse/lights");
    delay(1000);
    Serial.print("Subscribed: ");
    Serial.println(subscribed);
  }
  if (!client->loop()) {
    subscribed = false;
    Serial.println("Client loop failed, reconnecting.");
    mqtt_connect();
    subscribed = client->subscribe("greenhouse/lights");
    delay(1000);
    Serial.print("Subscribed: ");
    Serial.println(subscribed);
  }
  timer.tick();
  client->publish("greenhouse/temperature", temperatureChar);
  client->publish("greenhouse/humidity", humidityChar);
  client->publish("greenhouse/dli", dliChar);
  client->publish("greenhouse/par", parChar);
  //client->publish("greenhouse/lights", (char*)lights_on);
  delay(TIME_INTERVAL); // Delay a second between loops.
} // End of loop

