/*
 * Created by Agoston Walter on 13 November, 2020
 * Notebook sensor uses ESP32 Adafruit Feather with push button and toggle button
 */

//general libraries
#include "Arduino.h"

// #include Sd card libraries
#include <SPI.h>
#include <SD.h>

// #include RTC libraries
#include <Wire.h>
#include "RTClib.h"

// #include Wificlock libraries (specific to ESP32)
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// #include accel libraries
// #include <Wire.h> (already included by RTC library)
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

//OOCSI libraries
#include "OOCSI.h"

//global variables
#define TOGGLE_LED_DELAY 10000
#define PUSHBUTTON_LED_DELAY 10000

//general global variables
int toggle_lastState = 0;
int toggle_savedState = LOW;
int pushButton_lastState = 0;
int pushbutton_savedState = LOW;


// time is measured in milliseconds and will quickly exceed limitations of an integer, so we use a long for these two
unsigned long toggle_lastDebounceTime = 0;  // the time the button state last switched
unsigned long pushButton_lastDebounceTime = 0;  // the time the button state last switched
unsigned long debounceDelay = 50;    // the state must remain the same for this many

//WiFi Data
const char* ssid = "Zeer stabiel zei de kat";
const char* password = "maarniemandkaneenkatverstaan";
const char* OOCSIName = "Vitality@Work/Notebook";
const char* hostserver = "oocsi.id.tue.nl";
OOCSI oocsi = OOCSI();

//SD card 
//variables
const int chipSelect = 33; //chip select for Adafruit feather connected to Wemos D1 micro SD card shield
char* fileName_pushButton = "NotebookData_pushButton.txt";
char* fileName_toggle = "NotebookData_toggle.txt";
File myFile;
//methods
void setupSD()
{
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
}
void writeSD(String value, char* fileName)
{
  myFile = SD.open(fileName, FILE_WRITE);
  if (myFile) 
  {
    myFile.println(value);
    myFile.close();
  Serial.println("done.");
  }
  else
  {
     Serial.println("error opening test.txt");
  }
}

//WiFiClock
//variables
const char* ntpServer = "pool.ntp.org";
int SysInitTime_UTC;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

//methods
void WiFiClock_connectWiFi()
{
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("CONNECTED");
  timeClient.begin();
}
void WiFiClock_calibrateSysTime()
{
  timeClient.update();
}

int WiFiClock_getTimeStamp()
{
  timeClient.update();
  Serial.println(timeClient.getEpochTime());
  return timeClient.getEpochTime();
}

//Toggle button
//variables
const int toggle_pinHigh = A1; //fill this in
const int toggle_pinLow = A0; //fill this in
const int toggle_pinLED =  A5;//fill this in
//methods
int toggle_takeMeasurement()
{
  if(digitalRead(toggle_pinHigh) == HIGH)
  {
    return 1;
  } 
  else{
    return 0;
  }
}

//push button
//variables
const int pushButton_pinA = 14;
const int pushButton_pinB = 13;
const int pushButton_pinLED = 21;

//methods
int pushButton_takeMeasurement()
{
  return digitalRead(pushButton_pinB);
}

//OOCSI Methods
void OOCSI_sendMessage_toggle(int timestamp_message, int toggle_state){
  oocsi.newMessage("Vitality@Work/Notebook/toggleData");
  oocsi.addString("device_id","dad4db3cedad1484b");
  oocsi.addInt("toggleState",toggle_state);
  oocsi.addInt("timestamp",timestamp_message);
  oocsi.sendMessage();
  oocsi.check();
}

void OOCSI_sendMessage_pushButton(int timestamp_message){
  oocsi.newMessage("Vitality@Work/Notebook/pushButtonData");
  oocsi.addString("device_id","d313f79a47f514ccb");
  oocsi.addInt("timestamp",timestamp_message);
  oocsi.sendMessage();
  oocsi.check();
}


void processOOCSI() {
  // don't do anything; we are sending only
}

void setup() {
  Serial.begin(9600);
  Serial.println("Serial initialized");
  // put your setup code here, to run once:
  setupSD();

  //initialize WiFiClock
  WiFiClock_connectWiFi();
  WiFiClock_calibrateSysTime();

  //initialize toggle pins
  pinMode(toggle_pinHigh,INPUT);
  pinMode(toggle_pinLow,INPUT);
  pinMode(toggle_pinLED,OUTPUT);

  //initialize pushButton pins
  pinMode(pushButton_pinA,OUTPUT);
  digitalWrite(pushButton_pinA,HIGH);
  pinMode(pushButton_pinB,INPUT);
  pinMode(pushButton_pinLED,OUTPUT);

  //connect to OOCSI
  oocsi.connect(OOCSIName, hostserver, ssid, password, processOOCSI);
//  oocsi.subscribe("Vitality@Work/pushButton1/TimeStampData");
}

void loop() {
  // put your main code here, to run repeatedly:
  
  //update states of toggle and push button
  int toggle_state = toggle_takeMeasurement(); //on is 1, off is 0
  int pushButton_state = pushButton_takeMeasurement();//pushed is 1, off is 0
  //logic
  if (toggle_state != toggle_lastState) 
  {
      toggle_lastDebounceTime = millis();
      Serial.println(toggle_state);
  }
  if ((millis() - toggle_lastDebounceTime) > debounceDelay)
  {
    if (toggle_savedState != toggle_state)
    {
      toggle_savedState = toggle_state;
//      write to SD
      writeSD(String(toggle_state), fileName_toggle);
      writeSD(",", fileName_toggle);
      writeSD(String(WiFiClock_getTimeStamp()), fileName_toggle);
      writeSD("\n", fileName_toggle);
//    
//    //send to OOCSI
      OOCSI_sendMessage_toggle(WiFiClock_getTimeStamp(), toggle_state);
      
    //flash LED
      digitalWrite(toggle_pinLED,HIGH);
      delay(TOGGLE_LED_DELAY);
      digitalWrite(toggle_pinLED,LOW);
      Serial.println("toggle changed");
    }
  }
  toggle_lastState = toggle_state;
  
  if (pushButton_state != pushButton_lastState) {
    pushButton_lastDebounceTime = millis();
    Serial.println("push Button debounced");
  }
  if ((millis() - pushButton_lastDebounceTime) > debounceDelay)
  {
    if (pushbutton_savedState != pushButton_state)
    {
      pushbutton_savedState = pushButton_state;
      if(pushButton_state == HIGH)
      {
//        write to SD
        writeSD(String(WiFiClock_getTimeStamp()), fileName_pushButton);
      
        //send to OOCSI
        OOCSI_sendMessage_pushButton(WiFiClock_getTimeStamp());
      
        //flash LED
        digitalWrite(pushButton_pinLED,HIGH);
        delay(PUSHBUTTON_LED_DELAY);
        digitalWrite(pushButton_pinLED,LOW);
        Serial.println("push button changed");
      }
    } 
  }
  pushButton_lastState = pushButton_state;
}
