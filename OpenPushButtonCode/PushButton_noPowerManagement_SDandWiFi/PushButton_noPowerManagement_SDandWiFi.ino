/*
 For Vitality@Work Project, open sensor, push button, code
 last edited by Agoston Walter on October 11, 2020
 Based on Push Button example for Wemos D1, Examples/LOLIN D1 EXAMPLES/0.4 Shields/1_ButtonShield/DebounceToggleLED
 Also references OOCSI_sender example, from OOCSI library written by Matthias Funk
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
const char* ssid = "tue-psk";
const char* password = "!Demoday1";
const char* OOCSIName = "Vitality@Work/pushButton1";
const char* hostserver = "oocsi.id.tue.nl";
OOCSI oocsi = OOCSI();

//initialize pins
const int buttonPin = D3;
const int chipSelect = D8;

//initialize SD file
File myFile;

//initialize TimeStamp
char TimeStampBuf[40];

// the current state of the button
int buttonState = LOW;
int resetFlag = HIGH; //account for D3 being pulled up, automatically registering timestamp on boot

// the current and previous readings from the input pin
int thisButtonState = LOW;
int lastButtonState = LOW;

// time is measured in milliseconds and will quickly exceed limitations of an integer, so we use a long for these two
unsigned long lastDebounceTime = 0;  // the time the button state last switched
unsigned long debounceDelay = 50;    // the state must remain the same for this many millis to register the button press

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
//    Serial.print(now.year(), DEC);
//    Serial.print('/');
//    Serial.print(now.month(), DEC);
//    Serial.print('/');
//    Serial.print(now.day(), DEC);
//    Serial.print(" (");
//    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
//    Serial.print(") ");
//    Serial.print(now.hour(), DEC);
//    Serial.print(':');
//    Serial.print(now.minute(), DEC);
//    Serial.print(':');
//    Serial.print(now.second(), DEC);
//    Serial.println();

//    byte currHour = now.hour();
//    byte currMinute = now.minute();
//    byte currSecond = now.second();

    sprintf(TimeStampBuf, "%02d/%02d/%02d(%s)-%02d:%02d:%02d", now.year(),now.month(),now.day(),daysOfTheWeek[now.dayOfTheWeek()],now.hour(), now.minute(), now.second());
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

void OOCSI_sendMessage(){
  oocsi.newMessage("Vitality@Work/pushButton1/TimeStampData");
//  String TimeStampString;
//  TimeStampString += TimeStampBuf;
  oocsi.addString("device_id","d313f79a47f514ccb");
  oocsi.addString("timestamp",TimeStampBuf);
  oocsi.sendMessage();
  oocsi.printSendMessage();
  oocsi.check();
}

void setupRTC(){
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void setupSD(){
  //setup for SD card
  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
}

void processOOCSI() {
  // don't do anything; we are sending only
}


void setup() {
  //general setup
  Serial.begin(9600);
  Serial.print("Serial initialized");
//  WiFiOff(); // turn off WiFi

  //setup Modules: RTC, SD card
  setupRTC();
  setupSD();

  //setupPins
  pinMode(buttonPin, INPUT);
  // output OOCSI activity on the built-in LED, comment out for final version
//  pinMode(LED_BUILTIN, OUTPUT);
//  oocsi.setActivityLEDPin(LED_BUILTIN);

  // use this to switch off logging to Serial
  // oocsi.setLogging(false);

  // setting up OOCSI. processOOCSI is the name of the function to call when receiving messages, can be a random function name
  // connect wifi and OOCSI to the server
  oocsi.connect(OOCSIName, hostserver, ssid, password, processOOCSI);
  oocsi.subscribe("Vitality@Work/pushButton1/TimeStampData");
}

void loop() {
  thisButtonState = digitalRead(buttonPin);
  if (thisButtonState != lastButtonState) {
    lastDebounceTime = millis();
//    Serial.println("Debounced");
//    Serial.println(thisButtonState);
//    Serial.println(lastButtonState);
  }
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (thisButtonState != buttonState) {
      buttonState = thisButtonState;
      if (buttonState == HIGH) {
        if(!resetFlag){
          getTimeStamp();
          Serial.println(TimeStampBuf);
  //      WriteToSD(TimeStamp);
          OOCSI_sendMessage();  
        }
        else if (resetFlag){
          resetFlag = LOW;
        }
      }
    }
  }
  lastButtonState = thisButtonState;
}
