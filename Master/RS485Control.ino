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
  // Отправляем 1цу пока не получим двойку
  byte fromBridge = 0;
  while (fromBridge != 2)
  {
    digitalWrite(SSerialTxControl, HIGH);  // Init Transmitter
    delay(5);
    byte toBridge = 1;
    Serial1.println(toBridge);
    delay(5);
    digitalWrite(SSerialTxControl, LOW);  // Init Transmitter
    delay(500);
    if (Serial1.available() > 0)
    {
      fromBridge = Serial1.read();
    }
  }
  /*
    boolean sync = false;
    boolean timeOut = false;
    boolean recieved = false;
    unsigned long connTime = millis();
    unsigned long lastRec = connTime;
    byte connCount = 0;
    while (!sync && !timeOut) // пока не синхронизировались и не вышло время:
    { // отправляем сигнал пока не получили в ответ
    // получили ответ - ждем паузы сигналов и стартуем игру
    unsigned long syncTick = millis();
    if (recieved)
    {
      if(syncTick - lastRec > 2000) sync = true;
    }
    else
    {
      digitalWrite(SSerialTxControl, HIGH);  // Init Transmitter
      delay(5);
      Serial1.println("startMaster");
      delay(5);
      digitalWrite(SSerialTxControl, LOW);  // Init Transmitter
      delay(500);
    }
    if (Serial1.available() > 0)
    {
      String rec = Serial1.readStringUntil('\n');
      if (rec.indexOf("Bridge") > 0)
      {
        recieved = true;
        lastRec = syncTick;
      }
    }
    if (syncTick - connTime > 1000)
    {
      if (++connCount > 30) timeOut = true;
      Serial.print(String(connCount) + "..");
      connTime += 1000;
    }
    }
    if (sync) Serial.println("OK");
    if (timeOut) Serial.println("timeOut");
  */
}

