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
        if (input[i]) operGStates[i] = true;
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
  for (int d = 0; d < 31; d++)
  {
    if (passGStates[d]) Serial1.write(0x01);
    else Serial1.write(0x00);
    if (!operGStates[d] && passGStates[d])
    {
      Serial.println("Gadget " + String(gadgetNames[d]) + " passed by player");
      operGStates[d] = true;
    }
    delay(2);
  }
  Serial1.write(0xFF);
  digitalWrite(SSerialTxControl, LOW);  // Init Transmitter
}

void connectToBridge()
{
  boolean timeOut = false;
  unsigned long connTime = millis();
  byte connCount = 0;
  Serial1.flush();
  while (!Serial1.available()) {
    ;
  }
  while (Serial1.available() && !timeOut)
  {
    String rec = Serial1.readStringUntil('\n');
    if (rec.indexOf("Bridge") > 0)
    {
      digitalWrite(SSerialTxControl, HIGH);  // Init Transmitter
      delay(5);
      Serial1.println("startMaster");
      delay(5);
      digitalWrite(SSerialTxControl, LOW);  // Init Transmitter
    }
    if (millis() - connTime > 1000)
    {
      if (++connCount > 10)
      {
        Serial.println("timeout");
        timeOut = true;
      }
      Serial.print(String(connCount) + "..");
      connTime += 1000;
    }
    delay(100);
  }
  delay(2000);
  Serial.println("OK");
}

