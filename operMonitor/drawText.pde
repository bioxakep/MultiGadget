float[] getAcsDes(String textStr, int textSize, color textC)
{
  textFont(timerFont, textSize);
  float textWidth = textWidth(textStr);
  float textX = width/2 - textWidth/2;
  float textY = height/2;
  float[] ascdes = {0,0};
  background(0);
  fill(textC);
  text(textStr, textX, textY);
  print("printed");
  redraw();
  float AcsY = textY;
  float DesY = textY;
  for(int i = int(textX); i < int(textX + textWidth); i++)
  {
    println(i);
    int yUP = int(textY) - int(textAscent());
    color cUP = get(i,yUP);
    while(cUP != textC)
    {
      yUP++;
      cUP = get(i,yUP);
   }
    println("calced Acs");
    if(yUP < AcsY) AcsY = yUP;
    
    int yDWN = int(textY) + int(textDescent());
    color cDWN = get(i,yDWN);
    while(cDWN != textC)
    {
      yDWN++;
      cDWN = get(i,yDWN);
    }
    if(yDWN > DesY) DesY = yDWN;
  }
  print("Ascent = " + str(AcsY));
  print("Descent = " + str(DesY));
  ascdes[0] = AcsY;
  ascdes[1] = DesY;
  return ascdes;
}
