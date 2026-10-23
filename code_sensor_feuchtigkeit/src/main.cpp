#include <Arduino.h>  
void setup() {
  
  Serial.begin(115200);
  
  Serial.println("ESP fertig");
}

void loop() {
  
  int sensorWert = analogRead(A0);
  Serial.print("Feuchtigkeitswert: ");
  Serial.println(sensorWert);

  
  delay(1000);
} 