#include <SoftwareSerial.h>

#define SSerialTxControl 3   //RS485 Direction control
SoftwareSerial mySerial(10, 11); // RX, TX
unsigned long sendTime = 0;
void setup() {
  // put your setup code here, to run once:
  Serial1.begin(9600);//RS-485 adapter
  Serial.begin(9600);
  pinMode(SSerialTxControl, OUTPUT);
  digitalWrite(SSerialTxControl, LOW); // Init Recieve RS485
}

void loop()
{
  unsigned long tick = millis();
  
  if (tick - sendTime > 2000)  
  {
    sendTime = tick;
    digitalWrite(SSerialTxControl, HIGH);
    Serial1.write(0xC2);
    delay(10);
    digitalWrite(SSerialTxControl, LOW);
    Serial.println("Send 0xС2 in " + String(tick));
    //while(Serial1.available()) Serial1.read();
  }
  if (Serial1.available() > 0)
  {
    while (Serial1.available())
    {
      byte inByte = Serial1.read();
      Serial.println("Recieved:"+String(inByte)+" in "+String(tick));
    }
  }
}
