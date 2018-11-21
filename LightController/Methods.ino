void setLightBri(int value)
{
  if (value >= 0 && value <= 255)
  {
    Serial.println("Light set to " + String(value) + " 0-255");
    analogWrite(oknoA_R, value);
    delay(50);
    analogWrite(oknoA_G, value);
    delay(50);
    analogWrite(oknoA_B, value);
    delay(50);

    analogWrite(oknoB_R, value);
    delay(50);
    analogWrite(oknoB_G, value);
    delay(50);
    analogWrite(oknoB_B, value);
    delay(50);

    analogWrite(oknoC_R, value);
    delay(50);
    analogWrite(oknoC_G, value);
    delay(50);
    analogWrite(oknoC_B, value);
    delay(50);

    analogWrite(dvor_W, 255-value);
    delay(50);
    analogWrite(dvor_R, 255-value);
    delay(50);
    analogWrite(dvor_G, 255-value);
    delay(50);
    analogWrite(dvor_B, 255-value);
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

void shiftPic() {
  digitalWrite(motor,HIGH);
  delay(50);
  digitalWrite(motor,LOW);
}
