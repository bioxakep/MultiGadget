void connectToMonitor()
{
  boolean monConnected = false;
  while (!monConnected)
  {
    if (Serial.available() > 0)
    {
      String input = "";
      input = Serial.readStringUntil('\n');
      if (input.startsWith("letsgame"))
      {
        Serial.println("start");
        delay(500);
        digitalWrite(13, HIGH);
        boolean sync = false;
        while (!sync)
        {
          if (Serial.available() > 0)
          {
            Serial.readStringUntil('\n');
            delay(50);
            Serial.println("start");
            delay(500);
          }
          else sync = true;
        }
        monConnected = true;
      }
    }
  }
}


void connectToMaster()
{
  // Ожидаем 1цу с мастера
  // При приеме 1цы отправляем 3 раза 2-ку и стартуем
  byte fromMaster = 0;
  byte toMaster = 2;
  while (fromMaster != 1)
  {
    if (masterSerial.available() > 0)
    {
      fromMaster = masterSerial.read();
    }
  }
  digitalWrite(serialTXControl, HIGH);  // Init Transmitter
  delay(5);
  masterSerial.println(toMaster);
  delay(100);
  masterSerial.println(toMaster);
  delay(5);
  digitalWrite(serialTXControl, LOW);  // Init Transmitter
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
      digitalWrite(serialTXControl, HIGH);  // Init Transmitter
      delay(5);
      masterSerial.println("startBridge");
      delay(5);
      digitalWrite(serialTXControl, LOW);  // Init Transmitter
      delay(500);
    }

    if (masterSerial.available() > 0)
    {
      String rec = masterSerial.readStringUntil('\n');
      if (rec.indexOf("Master") > 0)
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

