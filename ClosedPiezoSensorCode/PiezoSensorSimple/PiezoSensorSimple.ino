/******************************************************************************
Piezo_Vibration_Sensor.ino
Example sketch for SparkFun's Piezo Vibration Sensor
  (https://www.sparkfun.com/products/9197)
Jim Lindblom @ SparkFun Electronics
April 29, 2016

- Connect a 1Mohm resistor across the Piezo sensor's pins.
- Connect one leg of the Piezo to GND
- Connect the other leg of the piezo to A0

Vibrations on the Piezo sensor create voltags, which are sensed by the Arduino's
A0 pin. Check the serial monitor to view the voltage generated.

Development environment specifics:
Arduino 1.6.7
******************************************************************************/

#include <Arduino.h>
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "OOCSI.h"

// Set the pins used
#define cardSelect 4

RTC_PCF8523 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

/*initialize constants*/
//initialize wifi
const char* ssid = "tue-psk";
const char* password = "!Demoday1";
const char* OOCSIName = "Vitality@Work/PiezoSensorMat";
const char* hostserver = "oocsi.id.tue.nl";
OOCSI oocsi = OOCSI();

//initialize SD file
File myFile;

//initialize TimeStamp, acceleration Data array, and average acceleration
char TimeStampBuf[40];
int piezoADC_0;
int piezoADC_1;
int piezoADC_2;
int piezoADC_3;
float piezoV_0;
float piezoV_1;
float piezoV_2;
float piezoV_3;


const int PIEZO_PIN_0 = A2; // Piezo output
const int PIEZO_PIN_1 = A3; // Piezo output
const int PIEZO_PIN_2 = A4; // Piezo output
const int PIEZO_PIN_3 = A7; // Piezo output

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

void sd_setup(){
  // see if the card is present and can be initialized:
  if (!SD.begin(cardSelect)) {
    Serial.println("Card init. failed!");
  }
}

void rtc_setup(){
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (! rtc.initialized() || rtc.lostPower()) {
    Serial.println("RTC is NOT initialized, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    //
    // Note: allow 2 seconds after inserting battery or applying external power
    // without battery before calling adjust(). This gives the PCF8523's
    // crystal oscillator time to stabilize. If you call adjust() very quickly
    // after the RTC is powered, lostPower() may still return true.
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  // When the RTC was stopped and stays connected to the battery, it has
  // to be restarted by clearing the STOP bit. Let's do this to ensure
  // the RTC is running.
  rtc.start();
}

void getTimeStamp() {
    DateTime now = rtc.now();
    sprintf(TimeStampBuf, "%02d/%02d/%02d(%s)-%02d:%02d:%02d", now.year(),now.month(),now.day(),daysOfTheWeek[now.dayOfTheWeek()],now.hour(), now.minute(), now.second());
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

void sensorRead(){
  // Read Piezo ADC value in, and convert it to a voltage
  piezoADC_0 = analogRead(PIEZO_PIN_0);
  piezoADC_1 = analogRead(PIEZO_PIN_1);
  piezoADC_2 = analogRead(PIEZO_PIN_2);
  piezoADC_3 = analogRead(PIEZO_PIN_3);
  
  piezoV_0 = piezoADC_0 / 1023.0 * 5.0;
  piezoV_1 = piezoADC_1 / 1023.0 * 5.0;
  piezoV_2 = piezoADC_2 / 1023.0 * 5.0;
  piezoV_3 = piezoADC_3 / 1023.0 * 5.0;
 
  Serial.print("piezoV_0:");Serial.print(piezoV_0);Serial.print(", ");
  Serial.print("piezoV_1:");Serial.print(piezoV_1);Serial.print(", ");
  Serial.print("piezoV_2:");Serial.print(piezoV_2);Serial.print(", ");
  Serial.print("piezoV_3:");Serial.print(piezoV_3);Serial.print(", ");
  Serial.println();  
}

void processOOCSI(){
  
}

void setup() 
{
  Serial.begin(9600);
  Serial.println("\r\nAnalog logger test");
//  pinMode(PIEZO_PIN_0,INPUT);
//  pinMode(PIEZO_PIN_1,INPUT);
//  pinMode(PIEZO_PIN_2,INPUT);
  pinMode(PIEZO_PIN_3,INPUT);
//  rtc_setup();
//  sd_setup();

  // output OOCSI activity on the built-in LED, comment out for final version
//  pinMode(LED_BUILTIN, OUTPUT);
//  oocsi.setActivityLEDPin(LED_BUILTIN);

  // use this to switch off logging to Serial
//   oocsi.setLogging(false);

  // setting up OOCSI. processOOCSI is the name of the function to call when receiving messages, can be a random function name
  // connect wifi and OOCSI to the server
  oocsi.connect(OOCSIName, hostserver, ssid, password, processOOCSI);
  oocsi.subscribe("Vitality@Work/PiezoSensor");
}

void loop() 
{
  sensorRead();
//  WriteToSD();
  OOCSI_sendMessage();
  delay(50);
}
