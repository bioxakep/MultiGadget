void pulseMeasure() //считываем пульс
{
  if (AFEXBEE.available() > 0)
  {
    String inStr = AFEXBEE.readStringUntil('\n');
    if (inStr[0] == 'A')
    {
      if (inStr[1] == '1') heartRate1 = HRLOW;
      else if (inStr[1] == '0') heartRate1 = 0;
      if (heartRate1 == 0 && (++HR1StopCount) > 1) 
      {
        HR1State = false;
        //Serial.println("Player 1 pulse off");
      }
      if (heartRate1 > 0) 
      { 
        HR1StopCount = 0; 
        HR1State = true;
        //Serial.println("Player 1 pulse on"); 
      }
    }
    
    if (inStr[0] == 'B')
    {
      if (inStr[1] == '1') heartRate2 = HRLOW;
      else if (inStr[1] == '0') heartRate2 = 0;
      if (heartRate2 == 0 && (++HR2StopCount) > 1) 
      {
        HR2State = false;
        //Serial.println("Player 2 pulse off");
      }
      if (heartRate2 > 0) 
      {
        HR2StopCount = 0;
        HR2State = true;
        //Serial.println("Player 2 pulse on");
      }
    }
    
  }
}
