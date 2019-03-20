void drawBreathe()
{
  println("br1Data="+str(br1Data) + " Min=" +str(br1Min) + " Max="+str(br1Max) + " RED = " + str(pl1BrDanger));
  println("br2Data="+str(br2Data) + " Min=" +str(br2Min) + " Max="+str(br2Max) + " RED = " + str(pl2BrDanger));
  float range1 = br1Max - br1Min;
  float range2 = br2Max - br2Min;
  
  if(range1 > 0) 
  {
    if(!pl1BrDanger)
    {
      stroke(0, 195, 206);
      fill(0, 195, 206);
    }
    else 
    {
      stroke(0, 195, 206);
      fill(redColor);
    }
    rect(brBarX+scrW/2, brBarY, brBarW - (br1Data-br1Min)*brBarW/range1, brBarH);
  }
  //else rect(brBarX, brBarY, brBarW/2, brBarH);
  if(range2 > 0) 
  {
    if(!pl2BrDanger) 
    {
      stroke(0, 195, 206);
      fill(0, 195, 206);
    }
    else
    {
      stroke(0, 195, 206);
      fill(redColor);
    }
    rect(brBarX, brBarY, brBarW - (br2Data-br2Min)*brBarW/range2, brBarH);
  }
  //else rect(brBarX, brBarY, brBarW/2, brBarH);
  fill(0, 195, 206);
}