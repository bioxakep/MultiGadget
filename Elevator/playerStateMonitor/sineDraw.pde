void drawSine(int bfr2, int bfr1)
{
  for (int s = 0; s < int(sineWidth) - 1; s++) // Рисуем все точки синусоиды 
  {
    line(scrW/4 + playerRectW/20 + s, sineValues1[s] + scrH/2 - playerRectH/6 - 2, scrW/4 + playerRectW/20 + s + 1, sineValues1[s+1] + scrH/2 - playerRectH/6 - 2);
    line(scrW/4 + playerRectW/20 + s + scrW/2, sineValues2[s] + scrH/2 - playerRectH/6 - 2, scrW/4 + playerRectW/20 + s + 1 + scrW/2, sineValues2[s+1] + scrH/2 - playerRectH/6 - 2);
    //ellipse()
  }
  ellipse(scrW/4 + playerRectW/20 + sineWidth,sineValues1[int(sineWidth) - 1] + scrH/2 - playerRectH/6 - 2, 4,4);
  ellipse(scrW/4 + playerRectW/20 + sineWidth + scrW/2,sineValues2[int(sineWidth) - 1] + scrH/2 - playerRectH/6 - 2, 4,4);
  //Смещение значений по всей ширине графика на 1 значение влево//
  for (int c = 0; c < int(sineWidth) - 1; c++)
  {
    sineValues1[c] = sineValues1[c+1];
    sineValues2[c] = sineValues2[c+1];
  }
  long t = millis();
  
  /////////////////////////////////////HEARTRATE 1  /////////////////////////////////////
  // Стартуем первый пик (маленький)
  if(startPeak1) 
  {
    float newValue1 = -sin(sineAlpha1) * sine1Amp;
    if(newValue1 > 0)
    {
      startPeak1 = false;
      startbigpause1 = true;
      sineAlpha1 = -PI/50;
      sine1Amp = bigPeakAmp;
      sineValues1[(int)sineWidth-1] = 0.0;
      pause1time = millis();
    }
    else
    {
      sineValues1[(int)sineWidth-1] = newValue1;
      sineAlpha1 += PI/5;
    }
  }
  // Пауза между пиками
  else if(startbigpause1)
  {
    sineValues1[(int)sineWidth-1] = 0.0;
    if(t - pause1time >= 70) { startbigpause1 = false; bigPeak1 = true; }
  }
  else if(bigPeak1)
  {
    float newValue1 = -sin(sineAlpha1) * sine1Amp;
    if(newValue1 > 0 && sineValues1[(int)sineWidth-2] < 0) // полный период нарисован - выходим.
    {
      bigPeak1 = false;
      lastPeak1 = true;
      sine1Amp = lastPeakAmp;
      sineAlpha1 = 0.0;
      sineValues1[(int)sineWidth-1] = 0.0;
    }
    else // Продолжаем вычислять синусоиду для пика
    {
      sineValues1[(int)sineWidth-1] = newValue1;
      sineAlpha1 += PI/10;
    }
  }
  else if(lastPeak1)
  {
    float newValue1 = sin(sineAlpha1) * sine1Amp;
    if(newValue1 < 0)
    {
      lastPeak1 = false;
      sine1Amp = startPeakAmp;
      sineAlpha1 = 0.0;
      sineValues1[(int)sineWidth-1] = 0.0;
    }
    else // Продолжаем вычислять синусоиду для пика
    {
      sineValues1[(int)sineWidth-1] = newValue1;
      sineAlpha1 += PI/8;
    }
  }
  
  /////////////////////////////////////HEARTRATE 2  /////////////////////////////////////
  if(startPeak2) 
  {
    float newValue2 = -sin(sineAlpha2) * sine2Amp;
    if(newValue2 > 0)
    {
      startPeak2 = false;
      startbigpause2 = true;
      sineAlpha2 = -PI/50;
      sine2Amp = bigPeakAmp;
      sineValues2[(int)sineWidth-1] = 0.0;
      pause2time = millis();
    }
    else
    {
      sineValues2[(int)sineWidth-1] = newValue2;
      sineAlpha2 += PI/5;
    }
  }
  // Пауза между пиками
  else if(startbigpause2)
  {
    sineValues2[(int)sineWidth-1] = 0.0;
    if(t - pause2time >= 70) { startbigpause2 = false; bigPeak2 = true; }
  }
  else if(bigPeak2)
  {
    float newValue2 = -sin(sineAlpha2) * sine2Amp;
    if(newValue2 > 0 && sineValues2[(int)sineWidth-2] < 0) // полный период нарисован - выходим.
    {
      bigPeak2 = false;
      lastPeak2 = true;
      sine2Amp = lastPeakAmp;
      sineAlpha2 = 0.0;
      sineValues2[(int)sineWidth-1] = 0.0;
    }
    else // Продолжаем вычислять синусоиду для пика
    {
      sineValues2[(int)sineWidth-1] = newValue2;
      sineAlpha2 += PI/10;
    }
  }
  else if(lastPeak2)
  {
    float newValue2 = sin(sineAlpha2) * sine2Amp;
    if(newValue2 < 0)
    {
      lastPeak2 = false;
      sine2Amp = startPeakAmp;
      sineAlpha2 = 0.0;
      sineValues2[(int)sineWidth-1] = 0.0;
    }
    else // Продолжаем вычислять синусоиду для пика
    {
      sineValues2[(int)sineWidth-1] = newValue2;
      sineAlpha2 += PI/8;
    }
  }
  
  if (bfr1 > 0)
  {
    if (t - time1 >= (long)(60000/bfr1) && !startPeak1 && !bigPeak1 && !lastPeak1) { 
      time1 = t;
      startPeak1 = true;
    }
  }
  if (bfr2 > 0)
  {
    if (t - time2 >= (long)(60000/bfr2) && !startPeak2 && !bigPeak2 && !lastPeak2) { 
      time2 = t; 
      startPeak2 = true;
    }
  }
}
