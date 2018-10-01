void getOperSkips()
{
  if (Serial1.available() > 0)
  {
    byte input[29];
    byte inByte = Serial1.read();
    if (inByte == 0xBB)
    {
      for (int i = 0; i < 29; i++)
      {
        input[i] = Serial1.read();
        if(input[i]) operGStates[i] = true;
      }
      byte last = Serial1.read();
      if(last == 0xFF) Serial.println("Operator Skips Recieved");
    }
    else Serial1.flush();
  }
}

void sendGStates() // Проверяем прошел ли игрок какой-нибудь гаджет
{
  byte chkSum = 0;
  digitalWrite(SSerialTxControl, HIGH);  // Init Transmitter
  Serial1.write(0xAA);
  for (int d = 0; d < 32; d++)
  {
    Serial1.write(passGStates[d]);
    if(!operGStates[d] && passGStates[d])
    {
      Serial.println("Gadget " + String(gadgetNames[d]) + " passed by player");
      operGStates[d] = true;
    }
    delay(2);
  }
  Serial1.write(0xFF);
  digitalWrite(SSerialTxControl, LOW);  // Init Transmitter
}
