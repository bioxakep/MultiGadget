void getBridgeData()
{
  if (Serial1.available() > 0)
  {
    byte input[32];
    byte inByte = Serial1.read();
    if (inByte == 0xBD)
    {
      Serial.print("Operator Skips Recieved:");
      delay(380);
      for (int i = 0; i < 32; i++)
      {
        input[i] = Serial1.read();
        if (input[i] == 0x03) operSkips[i] = true;
        Serial.print(input[i]);
        Serial.print("|");
      }
      byte last = Serial1.read();
      if (last == 0xFF) Serial.println("OK");
    }
    else if(inByte == 0xBF)
    {
      Serial.print("Light cmd recieved...");
      delay(100);
      byte lightCmd = Serial1.read();
      if(lightCmd == 0x01) digitalWrite(spare, LOW);
      else if(lightCmd == 0x05) digitalWrite(spare, HIGH);
      byte last = Serial1.read();
      if (last == 0xFF) Serial.println("OK");
    }
    else if (inByte == 0xBA) // if recieved voice-hints
    {
      delay(50);
      byte voiceHintIndex = Serial1.read();
      Serial.println("Recieved voiceHint# "+String(voiceHintIndex,HEX));
      if(voiceHintIndex >= 0 && voiceHintIndex < 34)
      {
        Serial.println("Voice hint of gadget " + String(voiceHintIndex) + " number "+ String(voiceHints[voiceHintIndex]) + " started");
        mp3_set_serial(Serial3);
        playVoice(voiceHintIndex);
      }
      byte last = Serial1.read();
      if (last == 0xFF) Serial.println("Voice hints recieved.");
    }
    else if (inByte == 0xBC) {
      Serial.println("Resync with Bridge...");
      bridgeConnected = false;
      resetStates();
      connectToBridge();
    }
    else if (inByte == 0xCF)
    {
      resetStates();
    }
    else Serial1.flush();
  }
}

void sendGStates() // Проверяем прошел ли игрок какой-нибудь гаджет
{
  byte chkSum = 0;
  boolean needSend = false;
  for (int s = 0; s < 32; s++)
  {
    if ((playerGDone[s] || operSkips[s]) && !gStates[s]) 
    {
      if (playerGDone[s]) needSend = true;
      gStates[s] = true;
      operSkips[s] = false;
    }
  }
  if (needSend)
  {
    digitalWrite(SSerialTxControl, HIGH);  // Init Transmitter
    delay(10);
    Serial1.write(0xAD);
    delay(10);
    Serial.print("Send States to Operator: ");
    for (int d = 0; d < 32; d++)
    {
      if (gStates[d] && !playerGDone[d]) Serial1.write(0x03);
      else if (gStates[d] && playerGDone[d]) Serial1.write(0x05);
      else Serial1.write(0x01);
      delay(10);
      operSkips[d] = false;
      if (gStates[d] && !playerGDone[d]) Serial.print('3');
      else if (gStates[d] && playerGDone[d]) Serial.print('5');
      else Serial.print('1');
    }
    Serial1.write(0xFF);
    delay(10);
    digitalWrite(SSerialTxControl, LOW);  // Stop Transmitter
    Serial.println(", level = " + String(level)); // DEBUG
    lcd.setCursor(0,0);
    lcd.print("SENT on ");
    lcd.print(millis());
  }
}

void connectToBridge()
{
  byte inByte = 0;
  byte outByte = 0xA1;
  boolean recieved = false;
  unsigned long tick = millis();
  unsigned long sendTime = tick;
  Serial.print("Connecting to Bridge..");
  while (!bridgeConnected)
  {
    sendTime = tick;
    digitalWrite(SSerialTxControl, HIGH);
    Serial1.write(outByte);
    delay(10);
    digitalWrite(SSerialTxControl, LOW);
    
    while (tick - sendTime < 1500 && !recieved)
    {
      tick = millis();
      if (Serial1.available() > 0)
      {
        while (Serial1.available())
        {
          byte inByte = Serial1.read();
          //if (inByte > 0) Serial.println("Recieved:" + String(inByte) + " in " + String(tick));
          if (inByte == 0xA1 && outByte == 0xA1) {
            outByte = 0xA2;
            Serial.print("1..");
          }
          if (inByte == 0xA2 && outByte == 0xA2) {
            outByte = 0xA3;
            Serial.print("2..");
          }
          if (inByte == 0xA3) bridgeConnected = true;
          lcd.clear();
          lcd.print("RCV:" + String(inByte));
          lcd.setCursor(0, 1);
          lcd.print(String(tick));
        }
      }
    }
  }
  lastA9SentTime = tick;
  Serial.println("connected");
}

void resetStates()
{
  for (int g = 0; g < totalGadgets; g++)
  {
    operSkips[g] = false;
    playerGDone[g] = false;
    gStates[g] = false;
    voiceHints[g] = 0;
  }
  level = 10;
  start = 0;
  thunderDone = false;
  sealsDone = false;
  shieldDone = false;
  underRFWait = true;
  windRFWait = true;
  rainRFWait = true;
  gateRFWait = true;
}
