/*
 * Created by Agoston Walter on 13 November, 2020
 * Notebook sensor uses ESP32 Adafruit Feather with push button and toggle button
 */

#define TOGGLE_LED_DELAY 10000
#define PUSHBUTTON_LED_DELAY 10000

#include "WorkVitalitySensor.h";

//global variables
int toggle_lastState = 0;
int pushButton_lastState = 0;

SD_card SD_card;
WiFiClock WiFiClock;
Toggle toggle;
PushButton pushButton;
WiFiData WiFiData;
OOCSIData OOCSIData;

void setup() {
  // put your setup code here, to run once:
  
  //sd card
  SD_card.chipSelect = 33; //chip select for Adafruit feather connected to Wemos D1 micro SD card shield
  SD_card.fileName = "NotebookData.txt";
  SD_card.setupSD();

  //initialize WiFiClock
  WiFiClock.connectWiFi();
  WiFiClock.calibrateSysTime();
  
  //oocsi data
  OOCSIData.OOCSIName = "Vitality@Work/Notebook";

  //initialize toggle
  toggle.pinHigh = A1; //fill this in
  toggle.pinLow = A0; //fill this in
  toggle.pinLED =  A4;//fill this in

  //initialize push button
  pushButton.pinA = 14;
  pushButton.pinB = 13;
  pushButton.pinLED = 21;

  //connect to OOCSI
  //put OOCSI Code here
}

void loop() {
  // put your main code here, to run repeatedly:
  
  //update states of toggle and push button
  int toggle_state = toggle.takeMeasurement(); //on is 1, off is 0
  int pushButton_state = pushButton.takeMeasurement();//pushed is 1, off is 0

  //logic
  if(toggle_state != toggle_lastState)
  {
    //write to SD
//    SD_card.writeSD(toggle_state);
//    SD_card.writeSD(",");
//    SD_card.writeSD(WiFiClock.getTimeStamp());
//    SD_card.writeSD("\n")
    
    //send to OOCSI
    //send to OOCSI method here
    
    //flash LED
    digitalWrite(toggle.pinLED,HIGH);
    delay(TOGGLE_LED_DELAY);
    digitalWrite(toggle.pinLED,LOW);

    //update state
    toggle_lastState = toggle_state;
  }
  else if(pushButton_state != pushButton_lastState)
  {
    //write to SD
//    SD_card.writeSD(WiFiClock.getTimeStamp());
//    SD_card.writeSD("\n");
    
    //send to OOCSI
    //send to OOCSI method here
    
    //flash LED
    digitalWrite(pushButton.pinLED,HIGH);
    delay(PUSHBUTTON_LED_DELAY);
    digitalWrite(pushButton.pinLED,LOW);

    //update state
    pushButton_lastState = pushButton_state;
  }
}
