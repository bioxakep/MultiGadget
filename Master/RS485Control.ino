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
  Serial1.write(0xAA);
  Serial.print("Send States to Operator: ");
  for (int d = 0; d < 31; d++)
  {
    if (passGStates[d]) Serial1.write(0x05);
    else Serial1.write(0x01);
    //Serial.print(passGStates[d]);
    if (!operGStates[d] && passGStates[d])
    {
      Serial.println("Gadget " + String(gadgetNames[d]) + " passed by player");
      operGStates[d] = true;
    }
    delay(5);
  }
  Serial.println();
  Serial1.write(0xFF);
  digitalWrite(SSerialTxControl, LOW);  // Init Transmitter
}

void connectToBridge()
{
  boolean recieved = false;
  int recCount = 0;
  unsigned long recTime = 0;
  while (!bridgeConnected)
  {
    unsigned long connTick = millis();
    if (Serial1.available() > 0)
    {
      recCount++;
      String input = "";
      input = Serial1.read();
      delay(50);
      if (input == 0xC2)
      {
        recTime = connTick;
        digitalWrite(SSerialTxControl, HIGH);  // Init Transmitter
        Serial1.write(0xC1);
        digitalWrite(SSerialTxControl, LOW);  // Init Transmitter
        lcd.clear();
        lcd.print("BRIDGE REC");
        delay(500);
        recieved = true;
      }
      if(input == 0xC3) bridgeConnected = true;
    }
    if (recieved && connTick - recTime > 1000) bridgeConnected = true;
  }
}

