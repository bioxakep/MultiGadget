void getOperSkips()
{
  if (Serial1.available() > 0)
  {
    byte chkSum = 0;
    byte input[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
    byte check = Serial1.read();
    if (check == 0xFF)
    {
      for (int i = 0; i < 16; i++)
      {
        input[i] = Serial1.read();
        if (input[i] > 0) chkSum++; // Подсчитываем значение контрольной суммы
      }
      byte dataChkSum = Serial1.read(); // Считываем значение контрольной суммы, должна совпасть с подсчитанной.
      if (chkSum == dataChkSum)
      {
        for (int i = 0; i < 16; i++)
        {
          if (input[i] != passGStates[i]) operGStates[i] = input[i]; // Нужные изменения применяем
        }
      }
    }
  }
  return false;
}

boolean sendGStates() // Проверяем прошел ли игрок какой-нибудь гаджет
{
  byte chkSum = 0;
  digitalWrite(SSerialTxControl, HIGH);  // Init Transmitter
  Serial1.write(0xFF);
  boolean sendStates[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
  for (int d = 0; d < 16; d++)
  {
    if (passGStates[d] && !operGStates[d])
    {
      operGStates[d] = true;
      sendStates[d] = true;
      chkSum++;
    }
    Serial1.write(sendStates[d]);
    delay(2);
  }
  Serial1.write(chkSum);
  Serial1.write(0xAA);
  digitalWrite(SSerialTxControl, LOW);  // Init Transmitter
}
