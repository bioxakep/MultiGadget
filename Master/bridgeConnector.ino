void getOperSkips()
{
  if (Serial1.available() > 0)
  {
    byte input[31];
    byte inByte = Serial1.read();

    if (inByte == 0xBB)
    {
      Serial.print("Operator Skips Recieved:");
      delay(350);
      for (int i = 0; i < 31; i++)
      {
        input[i] = Serial1.read();
        if (input[i] > 0x03) operGStates[i] = true;
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
    else if (inByte == 0xBD) // if recieved voice-hints
    {
      delay(50);
      byte voiceHintIndex = Serial1.read();
      Serial.println("Recieved voiceHint# "+String(voiceHintIndex,HEX));
      if(voiceHintIndex >= 0 && voiceHintIndex < 33)
      {
        Serial.println("Voice hint of gadget " + String(voiceHintIndex) + " number "+ String(voiceHintStates[voiceHintIndex]) + " started");
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
  for (int s = 0; s < 31; s++)
  {
    if (passGStates[s] && !operGStates[s])
    {
      needSend = true;
      operGStates[s] = true;
    }
  }
  if (needSend)
  {
    digitalWrite(SSerialTxControl, HIGH);  // Init Transmitter
    Serial.print("Send States to Operator: ");
    Serial1.write(0xAA);
    delay(10);
    for (int d = 0; d < 31; d++)
    {
      if (passGStates[d]) Serial1.write(0x05);
      else Serial1.write(0x01);
      delay(10);
      Serial.print(passGStates[d] ? 0x05 : 0x01); // DEBUG
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
    operGStates[g] = false;
    passGStates[g] = false;
    voiceHintStates[g] = 0;
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
