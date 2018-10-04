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
  boolean timeOut = false;
  unsigned long connTime = millis();
  byte connCount = 0;
  boolean sync = false;
  while (!sync && !timeOut)
  {
    delay(100);
    digitalWrite(serialTXControl, HIGH);  // Init Transmitter
    delay(5);
    masterSerial.println("startBridge");
    delay(5);
    digitalWrite(serialTXControl, LOW);  // Init Transmitter
    if (masterSerial.available() > 0)
    {
      String rec = masterSerial.readStringUntil('\n');
      if (rec.indexOf("Master") > 0) sync = true;
    }
    if (millis() - connTime > 1000)
    {
      if (++connCount > 30)
      {
        Serial.println("timeout");
        timeOut = true;
      }
      Serial.print(String(connCount) + "..");
      connTime += 1000;
    }
  }
  if (sync) Serial.println("OK");
}

