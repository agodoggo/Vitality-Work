/*
 * Created by Agoston Walter on 13 November, 2020
 * Piezo Mat sensor uses ESP32 Adafruit Feather with four piezo sensor in the floor mat
 */
#define SAMPLE_NO 20
#define SAMPLE_DUR 1000 //ms
#define SAMPLE_INT 50 //ms
#define QUAD_NO 4 //no of quadrants

 
void setup() {
  // put your setup code here, to run once:

  //create sensor
  WorkVitalitySensor mySensor;
  
  //sd card
  mySensor.SD_card.chipSelect = //edit this later; //chip select for Adafruit feather connected to Adafruit logger
  mySensor.SD_card.fileName = "piezoMatData.txt";
  mySensor.SD_card.setupSD();

  //initialize WiFiClock
  mySensor.WiFiClock.connectWiFi();
  mySensor.WiFiClock.calibrateSysTime();

  //oocsi data
  mySensor.OOCSIData.OOCSIName = "Vitality@Work/piezoMat";

  //piezo mat
  mySensor.piezoMat.pinPiezo1 = ; //fill this in
  mySensor.piezoMat.pinPiezo2 = ;
  mySensor.piezoMat.pinPiezo3 = ;
  mySensor.piezoMat.pinPiezo4 = ;

  //connect to OOCSI
  //put OOCSI Code here
}

void loop() {
  // put your main code here, to run repeatedly:

  //write timeStamp to SD
  mySensor.SD_card.writeSD(mySensor.WiFiClock.getTimeStamp());
  mySensor.SD_card.writeSD("\n");

  //get values from piezo Mat
  int tmpPiezoV[SAMPLE_NO][QUAD_NO];
  int tmpPiezoV_Avg[SAMPLE_DUR / SAMPLE_INT][QUAD_NO];
  
  for (int i = 0; i < SAMPLE_NO; i++){
    mySensor.piezoMat.takeMeasurement();
    for(int j = 0; j < SAMPLE_DUR / SAMPLE_INT; j++)
    {
      for(int k = 0; j < QUAD_NO; j++)
      {
        tmpPiezoV_Avg[j][k] = mySensor.piezoMat.piezoV[k] / (SAMPLE_DUR / SAMPLE_INT);  
      }
    }
    for(int l = 0; l < QUAD_NO; l++)
    {
        tmpPiezoV[i][l] = tmpPiezoV_Avg[i][l];
    }
    delay(SAMPLE_INT);
  }

  //write to SD and send to OOCSI
  for (int i = 0; i < SAMPLE_NO; i++)
  {
    for(int j = 0; j < QUAD_NO; j++)
    {
        //write to SD
        mySensor.SD_card.writeSD(tmpPiezoV[i][j]);
        mySensor.SD_card.writeSD("\n");

        //send to OOCSI
    }
  }
}
