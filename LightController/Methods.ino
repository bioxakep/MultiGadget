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
}

void randWind()
{
  tick = millis();
  if(windState && tick - startWind > windTime)
  {
    windDelay = random(minWindPause, maxWindPause);
    windState = false;
    stopWind = tick;
  }
  if(!windState && tick - stopWind > windDelay)
  {
    windTime = random(minWindDelay, maxWindDelay);
    windState = true;
    startWind = tick;
  }
  digitalWrite(wind, windState);
}

