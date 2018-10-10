#include <LiquidCrystal_I2C.h>
#define SSerialTxControl 17   //RS485 Direction control
LiquidCrystal_I2C lcd(0x3F,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
unsigned long sendTime = 0;
void setup() {
  // put your setup code here, to run once:
  Serial1.begin(9600);  //RS-485 adapter
  Serial.begin(9600);
  pinMode(19, INPUT_PULLUP);
  pinMode(SSerialTxControl, OUTPUT);
  digitalWrite(SSerialTxControl, LOW); // Init Recieve RS485
  lcd.init();                   
  lcd.backlight();
  lcd.print("Initialising...");
}

void loop() {
  if(!digitalRead(19)) Serial.println("RX WORKS");
  unsigned long tick = millis();
  /*
  if (tick - sendTime > 5672)  
  {
    sendTime = tick;
    digitalWrite(SSerialTxControl, HIGH);
    Serial1.write(0xC1);
    delay(10);
    digitalWrite(SSerialTxControl, LOW);
    Serial.println("Send 0xС1 in " + String(tick));
    while(Serial1.available()) Serial1.read();
  }
  */
  if (Serial1.available() > 0)
  {
    while (Serial1.available())
    {
      byte inByte = Serial1.read();
      Serial.println("Recieved:"+String(inByte)+" in "+String(tick));
      //lcd.clear();
      //lcd.print("RCV:"+String(inByte));
      //lcd.setCursor(0,1);
      //lcd.print(String(tick));
    }
  }
}
