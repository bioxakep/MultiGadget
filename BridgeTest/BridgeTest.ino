#include <SoftwareSerial.h>

#define SSerialTxControl 3   //RS485 Direction control
SoftwareSerial mySerial(10, 11); // RX, TX
unsigned long sendTime = 0;
void setup() {
  // put your setup code here, to run once:
  mySerial.begin(9600);//RS-485 adapter
  Serial.begin(9600);
  pinMode(SSerialTxControl, OUTPUT);
  digitalWrite(SSerialTxControl, LOW); // Init Recieve RS485
}

void loop()
{
  unsigned long tick = millis();
  /*
  if (tick - sendTime > 3287)  
  {
    sendTime = tick;
    digitalWrite(SSerialTxControl, HIGH);
    delay(200);
    mySerial.write(0xC2);
    delay(50);
    digitalWrite(SSerialTxControl, LOW);
    delay(200);
    Serial.println("Send 0xС2 in " + String(tick));
    while(mySerial.available()) mySerial.read();
  }
  */
  if (mySerial.available() > 0)
  {
    while (mySerial.available()) 
    {
      byte inByte = mySerial.read();
      Serial.println("Recieved:"+String(inByte)+" in "+String(tick));
    }
  }
}
