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
  boolean timeOut = false;
  unsigned long connTime = millis();
  byte connCount = 0;
  boolean sync = false;
  boolean recFromBridge = false;
  Serial1.flush();
  while (!sync && !timeOut)
  {
    unsigned long connTick = millis();
    if (connTick - connTime > 1000)
    {
      if (++connCount > 30) timeOut = true;
      Serial.print(String(connCount) + "..");
      connTime += 1000;
    }
    if(connTick - lastRec > 1000 && recFromBridge) sync = true;
    if (Serial1.available() > 0)
    {
      lastRec = connTick;
      String rec = Serial1.readStringUntil('\n');
      if (rec.indexOf("Bridge") > 0)
      {
        recFromBridge = true;
        delay(500);
        digitalWrite(SSerialTxControl, HIGH);  // Init Transmitter
        delay(5);
        Serial1.println("startMaster");
        delay(5);
        digitalWrite(SSerialTxControl, LOW);  // Init Transmitter
      }
    }
  }
  if (timeOut) Serial.println("...timeout.");
  else Serial.println("OK");
}

