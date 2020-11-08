/*
 * 1 Button Shield - Simple Push
 * Press the pushbutton to switch on the LED
 *
 * 1 Button Shield pushbutton connects pin D3 to GND
 */

#include "Arduino.h"
#include <ESP8266WiFi.h>

#define FPM_SLEEP_MAX_TIME           0xFFFFFFF
 
// Required for LIGHT_SLEEP_T delay mode
extern "C" {
#include "user_interface.h"
}

const int buttonPin = D3;
const int ledPin = BUILTIN_LED;

int timesWoken = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Exiting setup");
  delay(1000);
}

void callback() {
  Serial.println("Callback");
  Serial.flush();
}

#define LIGHT_WAKE_PIN D3

void enter_light_sleepMode() {
  Serial.println("Enter light sleep mode");
  gpio_pin_wakeup_enable(GPIO_ID_PIN(LIGHT_WAKE_PIN), GPIO_PIN_INTR_LOLEVEL);
  wifi_set_opmode(NULL_MODE);
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  wifi_fpm_open();
  wifi_fpm_set_wakeup_cb(callback);
  wifi_fpm_do_sleep(FPM_SLEEP_MAX_TIME);
  delay(1000);
}

void loop() {
  enter_light_sleepMode();
  timesWoken+=1;
  Serial.print("timesWoken: ");Serial.print(timesWoken);Serial.println();
  delay(10000);
  
}
