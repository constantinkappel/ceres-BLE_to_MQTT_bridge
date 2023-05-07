#ifndef LIGHTS
#define LIGHTS
#define LED_PIN 22

#include <esp32-hal-gpio.h>

extern const char* lights_on;
void setup_lights();

#endif // LIGHTS include goard