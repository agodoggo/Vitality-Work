/*
 * Created by Agoston Walter on 13 November, 202
 * Accelerometer sensor uses Wemos D1 connected in a shield
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
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// #include accel libraries
 #include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

//OOCSI libraries
#include "OOCSI.h"

//WiFi Data
const char* ssid = "Zeer stabiel zei de kat";
const char* password = "maarniemandkaneenkatverstaan";
const char* OOCSIName = "Vitality@Work/acc_userTest";
const char* hostserver = "oocsi.id.tue.nl";
OOCSI oocsi = OOCSI();

//define sampling constants
#define SAMPLE_NO 20
#define SAMPLE_DUR 1000 //ms
#define SAMPLE_INT 50 //ms
#define AXES_NO 3 //no of quadrants

//SD card 
//variables
const int chipSelect = D4; //chip select for Wemos D1 micro SD shield
char* fileName = "accData_userTest.txt";
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

//accelerometer
//define object
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

//variables
#define sizeOfAccArray 5

float accX[sizeOfAccArray] = {0,0,0,0,0};
float accY[sizeOfAccArray] = {0,0,0,0,0};
float accZ[sizeOfAccArray] = {0,0,0,0,0};

int samplingInterval = 1000; //ms

int samplingIntervalAvg = 50; //ms

//methods
void setupAccel()
{
  if(!accel.begin())
    {
      Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    }
  accel.setRange(ADXL345_RANGE_16_G);
}
void takeAccelMeasurement(){
  memset(accX,0,sizeof(accX));
  memset(accY,0,sizeof(accX));
  memset(accZ,0,sizeof(accX));
  for(int i = 0; i < sizeOfAccArray; i++)
  {
    for(int j = 0; j < 20; j++)
    {
      sensors_event_t event; 
      accel.getEvent(&event);
      accX[i] = event.acceleration.x;
      accY[i] = event.acceleration.y;
      accZ[i] = event.acceleration.z;
      delay(samplingIntervalAvg);
    }
   
    /* Display the results (acceleration is measured in m/s^2) */
    Serial.print("X: "); Serial.print(accX[i]); Serial.print("  ");
    Serial.print("Y: "); Serial.print(accY[i]); Serial.print("  ");
    Serial.print("Z: "); Serial.print(accZ[i]); Serial.print("  ");Serial.println("m/s^2 ");
//    writeSD(String(WiFiClock_getTimeStamp()), fileName);
//    writeSD(String(accX[i]), fileName);
//    writeSD(String(accY[i]), fileName);
//    writeSD(String(accZ[i]), fileName);
  }
}


//OOCSI Methods
void OOCSI_sendMessage(int timestamp_message, float accX[sizeOfAccArray], float accY[sizeOfAccArray], float accZ[sizeOfAccArray])
{
  oocsi.newMessage("Vitality@Work/acc_userTest/accData1");
  oocsi.addString("device_id","dcb08cf8cbf2c4f0b");
  oocsi.addInt("timestamp",timestamp_message);
  oocsi.addFloat("accX0",accX[0]);
  oocsi.addFloat("accY0",accY[0]);
  oocsi.addFloat("accZ0",accZ[0]);
  oocsi.addFloat("accX1",accX[1]);
  oocsi.addFloat("accY1",accY[1]);
  oocsi.addFloat("accZ1",accZ[1]);
  oocsi.addFloat("accX2",accX[2]);
  oocsi.addFloat("accY2",accY[2]);
  oocsi.addFloat("accZ2",accZ[2]);
  oocsi.addFloat("accX3",accX[3]);
  oocsi.addFloat("accY3",accY[3]);
  oocsi.addFloat("accZ3",accZ[3]);
  oocsi.addFloat("accX4",accX[4]);
  oocsi.addFloat("accY4",accY[4]);
  oocsi.addFloat("accZ4",accZ[4]);
  oocsi.sendMessage();
  oocsi.printMessage();
  oocsi.check();
}

void processOOCSI() {
  // don't do anything; we are sending only
}

 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Serial initialized");
  
  //initialize SD
//  setupSD();

  //begin accel
  setupAccel();
  
  //initialize WiFiClock
  WiFiClock_connectWiFi();
  WiFiClock_calibrateSysTime();

  //connect to OOCSI
  oocsi.connect(OOCSIName, hostserver, ssid, password, processOOCSI);
//  oocsi.subscribe("Vitality@Work/pushButton1/TimeStampData");

}

void loop() {
  // put your main code here, to run repeatedly:
  takeAccelMeasurement();
  OOCSI_sendMessage(WiFiClock_getTimeStamp(), accX, accY, accZ);
}
