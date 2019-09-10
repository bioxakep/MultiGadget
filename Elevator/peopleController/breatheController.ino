void breatheMeasure()
{
  int breatheData1 = analogRead(BRE1PIN);
  //breatheData1 = 50 + 40 * sin(millis()/150.0)/(millis()/1000);
  int breatheData2 = analogRead(BRE2PIN);
  //breatheData2 = 50 + 40 * cos(PI * millis() / 12000.0);
  if (breatheMinMax1[0] > breatheData1) breatheMinMax1[0] = breatheData1;
  if (breatheMinMax1[1] < breatheData1) breatheMinMax1[1] = breatheData1;
  if (breatheMinMax2[0] > breatheData2) breatheMinMax2[0] = breatheData2;
  if (breatheMinMax2[1] < breatheData2) breatheMinMax2[1] = breatheData2;

  int delta1 = abs(breatheData1 - breathePrevData1);
  int delta2 = abs(breatheData2 - breathePrevData2);
  //Serial.println("BD1=" + String(breatheData1) + " BD1Min=" + String(breatheMinMax1[0]) + " BD1Max=" + String(breatheMinMax1[1]) + " Delta1 = " + String(delta1));
  //Serial.println("BD2=" + String(breatheData2) + " BD2Min=" + String(breatheMinMax2[0]) + " BD2Max=" + String(breatheMinMax2[1]) + " Delta2 = " + String(delta2));
  int maxDelta1 = 0;
  int maxDelta2 = 0;
  // ============ PLAYER 1 =====================
  if (!player1BreatheStoped && !visOnly)
  {
    if (delta1 <= breatheOffset)
    {
      if (!noBreatheMode1)
      {
        breathePrevData1 = breatheData1;
        noBreatheMode1 = true;
        //Serial.println("STOP BREATHE 1 PLAYER");
      }
      else
      {
        delta1Array[breatheStopCount1 % 3] = delta1;
        //Serial.print("Delta1 Array::");
        for (int g = 0; g < 3; g++)
        {
          if (maxDelta1 < delta1Array[g]) maxDelta1 = delta1Array[g];
          //Serial.print(String(delta1Array[g]) + "|");
        }
        breatheStopCount1++;
        //if(breatheStopCount1 % 2 == 0) Serial.println("Player 1 hold breathe for " + String(breatheStopCount1/2) + " seconds..");
      }
    }
    else
    {
      //Serial.println("Player 1 start breathe");
      noBreatheMode1 = false;
      breatheStopCount1 = 0;
      breathePrevData1 = breatheData1;
      for (int g = 0; g < 3; g++) delta1Array[g] = 0;
    }
  }

  if (breatheStopCount1 > 25)
  {
    //Serial.println("Player 1 stop breathe");
    player1BreatheStoped = true;
    breatheStopCount1 = 0;
  }

  if (breatheStopCount1 > 2 && maxDelta1 < 3 && breatheData1 > 180)
  {
    //Serial.println("Player 1 ejected");
    pla1Ejected = true;
    player1BreatheStoped = true;
    breatheStopCount1 = 0;
  }
  // ============ PLAYER 2 =====================
  if (!player2BreatheStoped && !visOnly)
  {
    if (delta2 <= breatheOffset)
    {
      if (!noBreatheMode2)
      {
        breathePrevData2 = breatheData2;
        noBreatheMode2 = true;
        //Serial.println("STOP BREATHE 2 PLAYER");
      }
      else
      {
        delta2Array[breatheStopCount2 % 3] = delta2;
        //Serial.print("Delta2 Array::");
        for (int g = 0; g < 3; g++)
        {
          if (maxDelta2 < delta2Array[g]) maxDelta2 = delta2Array[g];
          //Serial.print(String(delta2Array[g]) + "|");
        }
        breatheStopCount2++;
        //if(breatheStopCount2 % 2 == 0) Serial.println("Player 2 hold breathe for " + String(breatheStopCount2/2) + " seconds..");
        //Serial.println("| BRCNT2=" + String(breatheStopCount2));
      }
    }
    else
    {
      //Serial.println("Player 2 start breathe");
      noBreatheMode2 = false;
      breatheStopCount2 = 0;
      breathePrevData2 = breatheData2;
      for (int g = 0; g < 3; g++) delta2Array[g] = 0;
    }
  }

  if (breatheStopCount2 > 25)
  {
    //Serial.println("Player 2 stop breathe");
    player2BreatheStoped = true;
    breatheStopCount2 = 0;
  }

  if (breatheStopCount2 > 2 && maxDelta2 < 3 && breatheData2 > 200)
  {
    //Serial.println("Player 2 ejected");
    pla2Ejected = true;
    player2BreatheStoped = true;
    breatheStopCount2 = 0;
  }
  /*
  if(pla1Ejected || breatheStopCount1 > 10 || player1BreatheStoped)
  {
    Serial.println("BD1State=" + String(!player1BreatheStoped) + " BD1=" + String(breathePrevData1)
                                        + " BD1Min=" + String(breatheMinMax1[0]) + " BD1Max=" + String(breatheMinMax1[1]) + " COL1=RED");
  }
  else Serial.println("BD1State=" + String(!player1BreatheStoped) + " BD1=" + String(breatheData1)
                                        + " BD1Min=" + String(breatheMinMax1[0]) + " BD1Max=" + String(breatheMinMax1[1]) + " COL1=NORM");
  if(pla2Ejected || breatheStopCount2 > 10 || player2BreatheStoped)
  {
    Serial.println("BD2State=" + String(!player2BreatheStoped) + " BD2=" + String(breathePrevData2)
                                        + " BD2Min=" + String(breatheMinMax2[0]) + " BD2Max=" + String(breatheMinMax2[1]) + " COL2=RED");
  }
  else Serial.println("BD2State=" + String(!player2BreatheStoped) + " BD2=" + String(breatheData2)
                                        + " BD2Min=" + String(breatheMinMax2[0]) + " BD2Max=" + String(breatheMinMax2[1]) + " COL2=NORM");
*/
}
