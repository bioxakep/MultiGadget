void lightOff()
{
  digitalWrite(oknoA_R, LOW);
  delay(50);
  digitalWrite(oknoA_G, LOW);
  delay(50);
  digitalWrite(oknoA_B, LOW);
  delay(50);
  
  digitalWrite(oknoB_R, LOW);
  delay(50);
  digitalWrite(oknoB_G, LOW);
  delay(50);
  digitalWrite(oknoB_B, LOW);
  delay(50);

  digitalWrite(oknoC_R, LOW);
  delay(50);
  digitalWrite(oknoC_G, LOW);
  delay(50);
  digitalWrite(oknoC_B, LOW);
  delay(50);

  digitalWrite(dvor_W, LOW);
  delay(50);
  digitalWrite(dvor_R, LOW);
  delay(50);
  digitalWrite(dvor_G, LOW);
  delay(50);
  digitalWrite(dvor_B, LOW);
  delay(50);
  Serial.println("Light is Off");
}

void randWind()
{
  unsigned long tick = millis();
  if(windState && tick - startWind > windTime)
  {
    windPause = random(minWindPause, maxWindPause);
    windState = false;
    stopWind = tick;
    Serial.println("Wind pause");
  }
  if(!windState && tick - stopWind > windPause)
  {
    windTime = random(minWindTime, maxWindTime);
    windState = true;
    startWind = tick;
    Serial.println("Wind start");
  }
  digitalWrite(wind, windState);
}

