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
  delay(10);
  for (int d = 0; d < 31; d++)
  {
    if (passGStates[d]) Serial1.write(0x05);
    else Serial1.write(0x01);
    delay(10);
    //Serial1.write(passGStates[d] ? 0x05 : 0x01);
    //Serial.print(passGStates[d] ? 0x05 : 0x01);
    if (!operGStates[d] && passGStates[d])
    {
      Serial.println("Gadget " + String(gadgetNames[d]) + " passed by player");
      operGStates[d] = true;
    }
  }
  Serial1.write(0xFF);
  delay(10);
  digitalWrite(SSerialTxControl, LOW);  // Stop Transmitter
  Serial.println();
  lcd.clear();
  lcd.print("SENT");
  
}

void connectToBridge()
{
  byte inByte = 0;
  byte outByte = 0xA1;
  boolean recieved = false;
  unsigned long tick = millis();
  unsigned long sendTime = tick;
  while (!bridgeConnected)
  {
    /*
       ждем 3 секунды
        если ничего не приняли - отправляет текущий байт
        если приняли - отправляем сразу байт ответ (меняем на следующий) и снова ждем
       после отправки приняли предыдущий байт - не меняем ничего.
       меняем и отправляем СРАЗУ только если приняли следующий
    */
    sendTime = tick;
    digitalWrite(SSerialTxControl, HIGH);
    Serial1.write(outByte);
    delay(10);
    digitalWrite(SSerialTxControl, LOW);
    Serial.println("Send " + String(outByte,HEX) + " in " + String(tick));
    
    while (tick - sendTime < 1500 && !recieved)
    {
      tick = millis();
      if (Serial1.available() > 0)
      {
        while (Serial1.available())
        {
          byte inByte = Serial1.read();
          Serial.println("Recieved:" + String(inByte) + " in " + String(tick));
          if (inByte == 0xA1 && outByte == 0xA1) { outByte = 0xA2; }
          if (inByte == 0xA2 && outByte == 0xA2) { outByte = 0xA3; }
          if (inByte == 0xA3) bridgeConnected = true;
          lcd.clear();
          lcd.print("RCV:" + String(inByte));
          lcd.setCursor(0, 1);
          lcd.print(String(tick));
        }
      }
    }
  }
}

