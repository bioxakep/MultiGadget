void connectToMonitor()
{
  while (!monitorConnected)
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
            delay(1000);
          }
          else sync = true;
        }
        monitorConnected = true;
      }
    }
  }
}
