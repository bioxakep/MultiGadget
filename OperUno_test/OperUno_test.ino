#include <SoftwareSerial.h>

#define SSerialTxControl 3   //RS485 Direction control
SoftwareSerial mySerial(11, 10); // RX, TX
void setup() {
  // put your setup code here, to run once:
  mySerial.begin(9600);//RS-485 adapter
  Serial.begin(9600);
  pinMode(SSerialTxControl, OUTPUT);
  digitalWrite(SSerialTxControl, LOW); // Init Recieve RS485
}

void loop() 
{
  if (mySerial.available() > 0)
  {
    while (mySerial.available()) 
    {
      inByte = mySerial.read();
      if(inByte == 0xFB)
      {
        digitalWrite(SSerialTxControl, HIGH);
        mySerial.write(0xFB);
        digitalWrite(SSerialTxControl, LOW);
      }
    }
  }
}
