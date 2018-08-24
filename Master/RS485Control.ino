void sendStates()
{
  digitalWrite(SSerialTxControl, HIGH);  // Init Transmitter
  rs485.write(0xFF);
  for (int i = 0; i < devCount; i++) rs485.write(curGStates[i]);
  rs485.write(0xAA);
  digitalWrite(SSerialTxControl, LOW);  // Init Transmitter
}


boolean getOperSkips()
{
  if (rs485.available() > 0)
  {
    byte input[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    byte check = rs485.read();
    byte chksum = 0;
    while(rs485.available() && check != 0xFF) check = rs485.read();
    if(check == 0xFF)
    {
      for (int i = 0; i < devCount - 1; i++) 
      {
        input[i] = rs485.read();
        chksum += input[i]; // Подсчитываем значение контрольной суммы
      }
      byte truechksum = rs485.read(); // Считываем значение контрольной суммы, должна совпасть с подсчитанной.
      for (int i = 0; i < devCount; i++) 
      {
        if(input[i] != curGStates[i] && chksum == truechksum) curGStates[i] = input[i]; // Нужные изменения применяем
      }
    }
  }
  return false;
}

boolean checkDevices() // Проверяем прошел ли игрок какой-нибудь гаджет
{
  boolean changed = false;
  for (int d = 0; d < devCount; d++)
  {
    if (currGStates[d] < newGStates[d])
    {
      changed = true;
      curGStates[d] = newGStates[d];
      newGStates[d] = 0;
    }
  }
  return changed;
}
