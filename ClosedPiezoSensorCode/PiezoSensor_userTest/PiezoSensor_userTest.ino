#include <Arduino.h>
#include <RTClib.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <OOCSI.h>

OOCSI oocsi = OOCSI();

void OOCSI_sendMessage(){
  oocsi.newMessage("Vitality@Work/PiezoSensor");
//  String TimeStampString;
//  TimeStampString += TimeStampBuf;
  oocsi.addString("device_id","d7931f349a0854065");
//  oocsi.addString("timestamp",TimeStampBuf);
  oocsi.addFloat("PiezoSensor0", piezoV_0);
  oocsi.addFloat("PiezoSensor1", piezoV_1);
  oocsi.addFloat("PiezoSensor2", piezoV_2);
  oocsi.addFloat("PiezoSensor3", piezoV_3);
  oocsi.sendMessage();
  oocsi.printSendMessage();
  oocsi.check();
}
void processOOCSI(){
  
}

void setup() 
{
  Serial.begin(9600);
  Serial.print("Serial initialized");

  //setup Modules: RTC, SD card, and accelerometer
//  Serial.println("Initializing RTC...");
//  myFeather_general.setupRTC();
  Serial.println("Initializing SD card...");
  myFeather_general.setupSD();
  myFeather_general.fileName = "piezoData.txt";

  // use this to switch off logging to Serial
//  oocsi.setLogging(false);

  // setting up OOCSI. processOOCSI is the name of the function to call when receiving messages, can be a random function name
  // connect wifi and OOCSI to the server
  oocsi.connect(myAccel.OOCSIName, myWEMOS_D1_general.hostserver, myWEMOS_D1_general.ssid, myWEMOS_D1_general.password, processOOCSI);
  oocsi.subscribe("Vitality@Work/Accelerometer1/AccelData");
}

void loop() 
{
  sensorRead();
//  WriteToSD();
  OOCSI_sendMessage();
  delay(50);
}
void WriteToSD(){
  myFile = SD.open("test.txt", FILE_WRITE);
  if (myFile) { 
    Serial.print("Writing to test.txt...");
    myFile.println("Timestamp: ");
    myFile.print(TimeStampBuf);
    myFile.println("Sensor 0:");
    myFile.print(piezoV_0);
    myFile.println("Sensor 1:");
    myFile.print(piezoV_1);
    myFile.println("Sensor 2:");
    myFile.print(piezoV_2);
    myFile.println("Sensor 3:");
    myFile.print(piezoV_3);
    myFile.print("_______________________");
    myFile.println();
    
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}
