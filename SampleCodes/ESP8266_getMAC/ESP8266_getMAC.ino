#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
 
void setup(){
 
   Serial.begin(115200);
   delay(500);
 
   Serial.println();
   Serial.print("MAC: ");
   Serial.println(WiFi.macAddress());
 
}
 
void loop(){}
