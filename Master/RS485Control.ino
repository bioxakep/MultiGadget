void getOperSkips()
{
  if (rs485.available() > 0)
  {
    byte chkSum = 0;
    byte input[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
    byte check = rs485.read();
    if (check == 0xFF)
    {
      for (int i = 0; i < 16; i++)
      {
        input[i] = rs485.read();
        if (input[i] > 0) chkSum++; // Подсчитываем значение контрольной суммы
      }
      byte dataChkSum = rs485.read(); // Считываем значение контрольной суммы, должна совпасть с подсчитанной.
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
  rs485.write(0xFF);
  boolean sendStates[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
  for (int d = 0; d < 16; d++)
  {
    if (passGStates[d] && !operGStates[d])
    {
      operGStates[d] = true;
      sendStates[d] = true;
      chkSum++;
    }
    rs485.write(sendStates[d]);
    delay(2);
  }
  rs485.write(chkSum);
  rs485.write(0xAA);
  digitalWrite(SSerialTxControl, LOW);  // Init Transmitter
}
