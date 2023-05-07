#include "lights.h"

const char* lights_on = "off";

void setup_lights() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  lights_on = "off";
}