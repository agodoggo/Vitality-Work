/*
 For Vitality@Work Project, open sensor, push button, code
 last edited by Agoston Walter on October 11, 2020
 Based on Push Button example for Wemos D1, Examples/LOLIN D1 EXAMPLES/0.4 Shields/1_ButtonShield/DebounceToggleLED
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include "OOCSI.h"

#define FPM_SLEEP_MAX_TIME 0xFFFFFFF

RTC_DS1307 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

/*initialize constants*/
//initialize wifi
const char* ssid = "Eaisi-ExperienceLab-2.4G";
const char* password = "ExperienceLabWifi";
//initialize pins
const int buttonPin = D3;
const int chipSelect = D8;

File myFile;

// the current state of the LED and button
int ledState = LOW;
int buttonState = LOW;

// the current and previous readings from the input pin
int thisButtonState = LOW;
int lastButtonState = LOW;

// time is measured in milliseconds and will quickly exceed limitations of an integer, so we use a long for these two
unsigned long lastDebounceTime = 0;  // the time the button state last switched
unsigned long debounceDelay = 50;    // the state must remain the same for this many millis to register the button press

void setup() {

  //general setup
  Serial.begin(9600);
  WiFiOff(); // turn off WiFi
  while (!Serial); // for Leonardo/Micro/Zero

  //setup for RTC module
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  //setup for pushbutton
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  //setup for SD card
  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // set initial LED state
  digitalWrite(ledPin, ledState);
}

void loop() {
  thisButtonState = digitalRead(buttonPin);
  if (thisButtonState != lastButtonState) {
    lastDebounceTime = millis();
  }
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (thisButtonState != buttonState) {
      buttonState = thisButtonState;
      if (buttonState == HIGH) {
        ledState = !ledState;
        digitalWrite(ledPin, ledState);
        getTimeStamp();
//      WriteToSD(TimeStamp);
      }
    }
  }
  lastButtonState = thisButtonState;
}

void WiFiOff() {
    // https://arduino.stackexchange.com/questions/43376/can-the-wifi-on-esp8266-be-disabled
    //Serial.println("diconnecting client and wifi");
    //client.disconnect();
    wifi_station_disconnect();
    wifi_set_opmode(NULL_MODE);
    wifi_set_sleep_type(MODEM_SLEEP_T);
    wifi_fpm_open();
    wifi_fpm_do_sleep(FPM_SLEEP_MAX_TIME);
}

void getTimeStamp() {
    DateTime now = rtc.now();
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
}

//void WriteToSD(char[] TimeStamp){
//  myFile = SD.open("test.txt", FILE_WRITE);
//
//  // if the file opened okay, write to it:
//  if (myFile) {
//    Serial.print("Writing to test.txt...");
//    myFile.println(TimeStamp);
//    // close the file:
//    myFile.close();
//    Serial.println("done.");
//  } else {
//    // if the file didn't open, print an error:
//    Serial.println("error opening test.txt");
//  }
//}
