#define SSerialTxControl 17   //RS485 Direction control
unsigned long tick = 0;
void setup() {
  // put your setup code here, to run once:
  Serial1.begin(9600);  //RS-485 adapter
  Serial.begin(9600);
  pinMode(SSerialTxControl, OUTPUT);
  digitalWrite(SSerialTxControl, LOW); // Init Recieve RS485
}

void loop() {
  if (millis() - tick > 5000)  
  {
    tick = millis();
    digitalWrite(SSerialTxControl, HIGH);
    Serial1.write(0xFB);
    digitalWrite(SSerialTxControl, LOW);
    Serial.println("Send in " + String(tick));
  }
  if (Serial1.available() > 0)
  {
    while (Serial1.available()) 
    {
      inByte = Serial1.read();
      if(inByte == 0xFB) Serial.println("Recieved in "+String(millis()));
    }
  }
}
