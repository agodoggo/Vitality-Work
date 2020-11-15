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

//WiFi Data
const char* ssid = "Zeer stabiel zei de kat";
const char* password = "maarniemandkaneenkatverstaan";
const char* OOCSIName = "Vitality@Work/PiezoMat_userTest10";
const char* hostserver = "oocsi.id.tue.nl";
OOCSI oocsi = OOCSI();

//SD card 
//variables
const int chipSelect = 33; //chip select for Adafruit feather connected to Wemos D1 micro SD card shield
char* fileName = "PiezoMat_userTest5Data.txt";
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
  while (WiFi.status() != WL_CONNECTED)
  {
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
//piezo mat
//pins
const int PIEZO_PIN_0 = A2;
const int PIEZO_PIN_1 = A3;
const int PIEZO_PIN_2 = A4;
const int PIEZO_PIN_3 = A7;

//variables
#define sizeOfPiezoArray 5

int piezoADC_0;
int piezoADC_1;
int piezoADC_2;
int piezoADC_3;

float piezoV_0[sizeOfPiezoArray];
float piezoV_1[sizeOfPiezoArray];
float piezoV_2[sizeOfPiezoArray];
float piezoV_3[sizeOfPiezoArray];

int samplingInterval = 1000; //ms

int samplingIntervalAvg = 50; //ms

//methods
void takePiezoMeasurement(){
  for(int i = 0; i < sizeOfPiezoArray; i++)
  {
    for(int j = 0; j < 20; j++)
    {
      piezoADC_0 = analogRead(PIEZO_PIN_0);
      piezoADC_1 = analogRead(PIEZO_PIN_1);
      piezoADC_2 = analogRead(PIEZO_PIN_2);
      piezoADC_3 = analogRead(PIEZO_PIN_3);

      piezoV_0[i] = piezoV_0[i] + piezoADC_0 / (1023.0 * 5.0) / 20;
      piezoV_1[i] = piezoV_1[i] + piezoADC_1 / (1023.0 * 5.0) / 20;
      piezoV_2[i] = piezoV_2[i] + piezoADC_2 / (1023.0 * 5.0) / 20;
      piezoV_3[i] = piezoV_3[i] + piezoADC_3 / (1023.0 * 5.0) / 20;  
      delay(samplingIntervalAvg);
    }
    Serial.print("piezoV_0:");Serial.print(piezoV_0[i]);Serial.print(", ");
    Serial.print("piezoV_1:");Serial.print(piezoV_1[i]);Serial.print(", ");
    Serial.print("piezoV_2:");Serial.print(piezoV_2[i]);Serial.print(", ");
    Serial.print("piezoV_3:");Serial.print(piezoV_3[i]);Serial.print(", ");
    Serial.println();
//    writeSD(String(WiFiClock_getTimeStamp()), fileName);
//    writeSD(String(piezoV_0[i]), fileName);
//    writeSD(String(piezoV_1[i]), fileName);
//    writeSD(String(piezoV_2[i]), fileName);
//    writeSD(String(piezoV_3[i]), fileName);
  }
}

//OOCSI Methods
void OOCSI_sendMessage(int timestamp_message, float piezoVoltageArr0[sizeOfPiezoArray], float piezoVoltageArr1[sizeOfPiezoArray], float piezoVoltageArr2[sizeOfPiezoArray], float piezoVoltageArr3[sizeOfPiezoArray])
{
  oocsi.newMessage("Vitality@Work/PiezoMat_userTest10/Data");
  oocsi.addString("device_id","d7931f349a0854065");
  oocsi.addInt("timestamp",timestamp_message);
  oocsi.addFloat("Quadrant0_0",piezoVoltageArr0[0]);
  oocsi.addFloat("Quadrant0_1",piezoVoltageArr0[1]);
  oocsi.addFloat("Quadrant0_2",piezoVoltageArr0[2]);
  oocsi.addFloat("Quadrant0_3",piezoVoltageArr0[3]);
  oocsi.addFloat("Quadrant1_0",piezoVoltageArr1[0]);
  oocsi.addFloat("Quadrant1_1",piezoVoltageArr1[1]);
  oocsi.addFloat("Quadrant1_2",piezoVoltageArr1[2]);
  oocsi.addFloat("Quadrant1_3",piezoVoltageArr1[3]);
  oocsi.addFloat("Quadrant2_0",piezoVoltageArr2[0]);
  oocsi.addFloat("Quadrant2_1",piezoVoltageArr2[1]);
  oocsi.addFloat("Quadrant2_2",piezoVoltageArr2[2]);
  oocsi.addFloat("Quadrant2_3",piezoVoltageArr2[3]);
  oocsi.addFloat("Quadrant3_0",piezoVoltageArr3[0]);
  oocsi.addFloat("Quadrant3_1",piezoVoltageArr3[1]);
  oocsi.addFloat("Quadrant3_2",piezoVoltageArr3[2]);
  oocsi.addFloat("Quadrant3_3",piezoVoltageArr3[3]);
  oocsi.sendMessage();
  oocsi.printMessage();
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

  //initialize piezo pins
  pinMode(PIEZO_PIN_0,INPUT);
  pinMode(PIEZO_PIN_1,INPUT);
  pinMode(PIEZO_PIN_2,INPUT);
  pinMode(PIEZO_PIN_3,INPUT);

  //connect to OOCSI
  oocsi.connect(OOCSIName, hostserver, ssid, password, processOOCSI);
//  oocsi.subscribe("Vitality@Work/pushButton1/TimeStampData");
}

void loop() {
  // put your main code here, to run repeatedly:
  takePiezoMeasurement();
  OOCSI_sendMessage(WiFiClock_getTimeStamp(), piezoV_0, piezoV_1, piezoV_2, piezoV_3);
}
