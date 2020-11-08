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
const int PIEZO_PIN_0 = A2; // Piezo output
const int PIEZO_PIN_1 = A3; // Piezo output
const int PIEZO_PIN_2 = A4; // Piezo output
const int PIEZO_PIN_3 = A7; // Piezo output

void setup() 
{
  Serial.begin(9600);
}

void loop() 
{
  // Read Piezo ADC value in, and convert it to a voltage
  sensorRead();
}
void sensorRead(){
  // Read Piezo ADC value in, and convert it to a voltage
  int piezoADC_0 = analogRead(PIEZO_PIN_0);
  int piezoADC_1 = analogRead(PIEZO_PIN_1);
  int piezoADC_2 = analogRead(PIEZO_PIN_2);
  int piezoADC_3 = analogRead(PIEZO_PIN_3);
  
  float piezoV_0 = piezoADC_0 / 1023.0 * 5.0;
  float piezoV_1 = piezoADC_1 / 1023.0 * 5.0;
  float piezoV_2 = piezoADC_2 / 1023.0 * 5.0;
  float piezoV_3 = piezoADC_3 / 1023.0 * 5.0;
 
  Serial.print("piezoV_0:");Serial.print(piezoV_0);Serial.print(", ");
  Serial.print("piezoV_1:");Serial.print(piezoV_1);Serial.print(", ");
  Serial.print("piezoV_2:");Serial.print(piezoV_2);Serial.print(", ");
  Serial.print("piezoV_3:");Serial.print(piezoV_3);Serial.print(", ");
  Serial.println();  
}
