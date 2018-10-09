unsigned long lastRec = 0;

void getOperSkips()
{
  if (Serial1.available() > 0)
  {
    byte input[31];
    byte inByte = Serial1.read();
    if (inByte == 0xBB)
    {
      for (int i = 0; i < 31; i++)
      {
        input[i] = Serial1.read();
        if (input[i] == 0x01) operGStates[i] = true;
        else operGStates[i] = false;
      }
      byte last = Serial1.read();
      if (last == 0xFF) Serial.println("Operator Skips Recieved");
    }
    else Serial1.flush();
  }
}

void sendGStates() // Проверяем прошел ли игрок какой-нибудь гаджет
{
  byte chkSum = 0;
  digitalWrite(SSerialTxControl, HIGH);  // Init Transmitter
  Serial.print("Send States to Operator: ");
  Serial1.write(0xAA);
  for (int d = 0; d < 31; d++)
  {
    if (passGStates[d]) Serial1.write(0x05);
    else Serial1.write(0x01);
    //Serial1.write(passGStates[d] ? 0x05 : 0x01);
    //Serial.print(passGStates[d] ? 0x05 : 0x01);
    if (!operGStates[d] && passGStates[d])
    {
      Serial.println("Gadget " + String(gadgetNames[d]) + " passed by player");
      operGStates[d] = true;
    }
  }
  Serial1.write(0xFF);
  Serial.println();
  lcd.clear();
  lcd.print("SENT");
  digitalWrite(SSerialTxControl, LOW);  // Init Transmitter
}

void connectToBridge()
{
  boolean recieved = false;
  int recCount = 0;
  unsigned long recTime = 0;
  while (!bridgeConnected)
  {
    digitalWrite(SSerialTxControl, HIGH);  // Init Transmitter
    delay(100);
    Serial1.write(0xC1);
    digitalWrite(SSerialTxControl, LOW);  // Init Transmitter
    delay(500);
    
    while (!Serial1.available()) {;}
    byte in = Serial1.read();
    lcd.clear();
    lcd.print("REC-ED");
    if (in == 0xC2) bridgeConnected = true;
    //if (recieved && connTick - recTime > 3500) bridgeConnected = true;
  }
}

