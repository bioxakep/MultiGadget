// WORLD_ADD v1 (beta)
// mini_Pro
// separate board for IRtx and HC-12 RX
#include <IRremote.h>

#include <SoftwareSerial.h>
SoftwareSerial mySerial(11, 10); // RX, TX HC-12

int irCmd  = 0xB88;  // trsfr  signal = 2952
//int irCmd  = 0xB99;  //full    signal = 2969
IRsend irsend;


int output   = A0;
int led      = 13;

void setup()
{
 Serial.begin(9600);
  mySerial.begin(9600); // HC-12 - signal from torch
 
 Serial.println("\n World_ADD_v1  AGO 2018");
 pinMode(output,OUTPUT); 
 pinMode(led,OUTPUT); 
 digitalWrite(led, LOW);
}

void loop() {
// if signal from torch via HC-12 (1 character message) then send signal to output
   
   if (millis() % 200 == 0) {
    irsend.sendRC5(irCmd & 0xFFF, 12); // signal for torch
    Serial.println(millis());
   }

   if (millis() % 333 == 0) {

   if(mySerial.available() > 1){    
    int input = mySerial.parseInt();//read serial input and convert to integer (-32,768 to 32,767)    
    if(input == 1111){//if on code is received
      digitalWrite(led   , HIGH);//turn LED on
      digitalWrite(output, HIGH);
      
    }
    if(input == 0000){//if off code is received
      digitalWrite(led   , LOW);//turn LED off
      digitalWrite(output, LOW);
     }
    } 
    mySerial.flush();    //clear the serial buffer for unwanted inputs     

   }

}
