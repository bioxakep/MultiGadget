void testB(){

digitalWrite(oknoA_G, HIGH); 
delay(1000);
digitalWrite(oknoA_G, LOW);
digitalWrite(oknoB_G, HIGH); 
delay(1000);
digitalWrite(oknoB_G, LOW);
digitalWrite(oknoC_G, HIGH); 
delay(1000);
digitalWrite(oknoC_G, LOW);
delay(1000);
}

void testA(){


if (digitalRead(crystPins[0]) ) {  digitalWrite(oknoA_G, HIGH); } else  { digitalWrite(oknoA_G, LOW); } 
if (digitalRead(crystPins[1]) ) {  digitalWrite(oknoB_G, HIGH); }  else { digitalWrite(oknoB_G, LOW); } 
if (digitalRead(crystPins[2]) ) {  digitalWrite(oknoC_G, HIGH); }  else { digitalWrite(oknoC_G, LOW);}
       

delay(300);
} 
 
void testing(){
  digitalWrite(wind,  HIGH);
  analogWrite(oknoA_R, 255);
  delay(99);
  analogWrite(oknoA_G, 255);
  delay(99);
  analogWrite(oknoA_B, 255);
  delay(99);

  analogWrite(oknoB_R, 255);
  delay(99);
  analogWrite(oknoB_G, 255);
  delay(99);
  analogWrite(oknoB_B, 255);
  delay(99);

  analogWrite(oknoC_R, 255);
  delay(99);
  analogWrite(oknoC_G, 255);
  delay(99);
  analogWrite(oknoC_B, 255);
  delay(99);

  analogWrite(dvor_W, 100);
  delay(99);
  analogWrite(dvor_R, 100);
  delay(99);
  analogWrite(dvor_G, 100);
  delay(99);
  analogWrite(dvor_B, 100);
  delay(99);

  digitalWrite(oknoA_R, LOW);
  delay(99);
  digitalWrite(oknoA_G, LOW);
  delay(99);
  digitalWrite(oknoA_B, LOW);
  delay(99);

  digitalWrite(oknoB_R, LOW);
  delay(99);
  digitalWrite(oknoB_G, LOW);
  delay(99);
  digitalWrite(oknoB_B, LOW);
  delay(99);

  digitalWrite(oknoC_R, LOW);
  delay(99);
  digitalWrite(oknoC_G, LOW);
  delay(99);
  digitalWrite(oknoC_B, LOW);
  delay(99);

  // LOW = ON
  digitalWrite(dvor_W, LOW); 
  delay(99);
  digitalWrite(dvor_R, LOW);
  delay(99);
  digitalWrite(dvor_G, LOW);
  delay(99);
  digitalWrite(dvor_B, LOW);
  delay(99);
  digitalWrite(wind,   LOW);

  Serial.println("Testing done.");
}
