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
  for (int d = 0; d < 31; d++)
  {
<<<<<<< HEAD
    Serial1.write(passGStates[d] & 0x01);
=======
    if (passGStates[d]) Serial1.write(0x01);
    else Serial1.write(0x00);
>>>>>>> 5516ac6acc5bd6ed047db6c54de7ee228e080321
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
