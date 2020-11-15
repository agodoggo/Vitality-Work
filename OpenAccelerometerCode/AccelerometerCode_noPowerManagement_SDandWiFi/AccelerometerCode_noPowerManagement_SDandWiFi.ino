/*
 For Vitality@Work Project, open sensor, push button, code
 last edited by Agoston Walter on November 10, 2020
 Based on Push Button example for Wemos D1, Examples/LOLIN D1 EXAMPLES/0.4 Shields/1_ButtonShield/DebounceToggleLED
 */

#include "Arduino.h"
#include "ESP8266WiFi.h"
#include "Adafruit_Sensor.h"
#include "Adafruit_ADXL345_U.h"
#include "SPI.h"
#include "SD.h"
#include "Wire.h"
#include "RTClib.h"
#include "OOCSI.h"

#include <WEMOS_D1_Accel.h>
#include <WEMOS_D1_general.h>

WEMOS_D1_general myWEMOS_D1_general;
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
WEMOS_D1_Accel  myAccel(accel);

//initialize wifi
OOCSI oocsi = OOCSI();

//void OOCSI_sendMessage(){
//  oocsi.newMessage("Vitality@Work/Accelerometer1/AccelData");
//  oocsi.addString("device_id","dcb08cf8cbf2c4f0b");
//  //actual data below
//  oocsi.addStringArray("timestamp",myAccel.timeStampArr,(sizeof(myAccel.timeStampArr)/sizeof(myAccel.timeStampArr[0])));
//  oocsi.addFloatArray("accDataX",myAccel.AccelX, (sizeof(myAccel.AccelX)/sizeof(myAccel.AccelX[0])));
//  oocsi.addFloatArray("accDataY",myAccel.AccelY, (sizeof(myAccel.AccelY)/sizeof(myAccel.AccelY[0])));
//  oocsi.addFloatArray("accDataZ",myAccel.AccelZ, (sizeof(myAccel.AccelZ)/sizeof(myAccel.AccelZ[0])));
//  oocsi.sendMessage();
//  // comment out for final version
//  oocsi.printSendMessage();
//  oocsi.check();
//  // set arrays back to 0
//  memset(myAccel.timeStampArr,0,sizeof(myAccel.timeStampArr));
//  memset(myAccel.AccelX,0,sizeof(myAccel.AccelX));
//  memset(myAccel.AccelY,0,sizeof(myAccel.AccelY));
//  memset(myAccel.AccelZ,0,sizeof(myAccel.AccelZ));
//}
void processOOCSI() {
  // don't do anything; we are sending only
}
void setup() {
  //Serial setup
  Serial.begin(9600);
  Serial.print("Serial initialized");

  //setup Modules: RTC, SD card, and accelerometer
  Serial.println("Initializing RTC...");
  myWEMOS_D1_general.setupRTC();
  Serial.println("Initializing SD card...");
  myWEMOS_D1_general.setupSD();
  myWEMOS_D1_general.fileName = "accData.txt";
  Serial.println("Initializing Accelerometer...");
  myAccel.setupAccel();

  // use this to switch off logging to Serial
//  oocsi.setLogging(false);

  // setting up OOCSI. processOOCSI is the name of the function to call when receiving messages, can be a random function name
  // connect wifi and OOCSI to the server
  oocsi.connect(myAccel.OOCSIName, myWEMOS_D1_general.hostserver, myWEMOS_D1_general.ssid, myWEMOS_D1_general.password, processOOCSI);
  oocsi.subscribe("Vitality@Work/Accelerometer1/AccelData");
}

void loop() {
  myAccel.takeAccelMeasurement();
  for(int i = 0; i < (sizeof(myAccel.timeStampArr)/sizeof(myAccel.timeStampArr[0]));i++){
    Serial.print("index: "); Serial.print(i);Serial.println();
    myWEMOS_D1_general.writeSD(myAccel.timeStampArr[i]);
    Serial.print("timeStamp: "); Serial.print(myAccel.timeStampArr[i]);Serial.println();
    myWEMOS_D1_general.writeSD(String(myAccel.AccelX[i]));
    Serial.print("accelX: "); Serial.print(myAccel.AccelX[i]);Serial.println();
    myWEMOS_D1_general.writeSD(String(myAccel.AccelY[i]));
    Serial.print("accelY: "); Serial.print(myAccel.AccelY[i]);Serial.println();
    myWEMOS_D1_general.writeSD(String(myAccel.AccelZ[i]));
    Serial.print("accelZ: "); Serial.print(myAccel.AccelZ[i]);Serial.println();
    Serial.println();
  }
//  OOCSI_sendMessage();
}
  
