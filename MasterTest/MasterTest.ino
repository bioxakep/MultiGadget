#define SSerialTxControl 17   //RS485 Direction control
unsigned long sendTime = 0;
void setup() {
  // put your setup code here, to run once:
  Serial1.begin(9600);  //RS-485 adapter
  Serial.begin(9600);
  pinMode(SSerialTxControl, OUTPUT);
  digitalWrite(SSerialTxControl, LOW); // Init Recieve RS485
}

void loop() {
  unsigned long tick = millis();
  if (tick - sendTime > 5672)  
  {
    sendTime = tick;
    digitalWrite(SSerialTxControl, HIGH);
    Serial1.write(0xC1);
    digitalWrite(SSerialTxControl, LOW);
    Serial.println("Send 0xFB in " + String(tick));
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
