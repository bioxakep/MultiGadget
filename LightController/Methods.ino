void setLightBri(int value)
{
  if (value >= 0 && value <= 255)
  {
    Serial.println("Light set to " + String(value) + " percent");
    digitalWrite(oknoA_R, value);
    delay(50);
    digitalWrite(oknoA_G, value);
    delay(50);
    digitalWrite(oknoA_B, value);
    delay(50);

    digitalWrite(oknoB_R, value);
    delay(50);
    digitalWrite(oknoB_G, value);
    delay(50);
    digitalWrite(oknoB_B, value);
    delay(50);

    digitalWrite(oknoC_R, value);
    delay(50);
    digitalWrite(oknoC_G, value);
    delay(50);
    digitalWrite(oknoC_B, value);
    delay(50);

    digitalWrite(dvor_W, value);
    delay(50);
    digitalWrite(dvor_R, value);
    delay(50);
    digitalWrite(dvor_G, value);
    delay(50);
    digitalWrite(dvor_B, value);
    delay(50);
  }
}
void randWind()
{
  unsigned long tick = millis();
  if (windState && tick - startWind > windTime)
  {
    windPause = random(minWindPause, maxWindPause);
    windState = false;
    stopWind = tick;
    Serial.println("Wind pause");
  }
  if (!windState && tick - stopWind > windPause)
  {
    windTime = random(minWindTime, maxWindTime);
    windState = true;
    startWind = tick;
    Serial.println("Wind start");
  }
  digitalWrite(wind, windState);
}

