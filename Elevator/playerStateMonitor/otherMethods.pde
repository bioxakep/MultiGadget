String getInput()
{
  if (arduino.available() > 0)
  {
    //println("start read string from serial::"+str(millis()));
    String inp = arduino.readStringUntil('\n');
    //println("end read string from serial::"+str(millis()));
    if (inp != null)
    {
      //if (inp.length() > 1) println(inp);
      //println("print string from serial::"+str(millis()));
      return inp;
    } else return " ";
  } else return " ";
}

// LEVEL 1 METHODS
void drawPlayerWindows()
{  
    stroke(0, 195, 206);
    fill(33, 34, 36);
    strokeWeight(3);
    for (int p = 0; p < 2; p++)
    {
      strokeWeight(2);
      rect(scrW/4 + p*scrW/2 - playerRectW/2, scrH/2 - playerRectH/2, playerRectW, playerRectH);//Big rect
      strokeWeight(1);
      rect(brBarX + p*scrW/2, brBarY, brBarW, brBarH);// BreatheBar
      //line(scrW/4 + playerRectW/20 + p*scrW/2, scrH/2 - playerRectH/6 - 2, scrW/4 + playerRectW/2 - playerRectW/20 + p*scrW/2, scrH/2 - playerRectH/6 - 2);
      //draw Man
      image(PMan, manX + p*scrW/2, manY, manW, manH);
      //draw Heart
      image(PHeart, heartX + p*scrW/2, heartY, heartW, heartH);
      //draw Breathe
      image(PBreathe, breatheX + p*scrW/2, breatheY, breatheW, breatheH);
    }
    textFont(prisonFont);
    textSize(18);
    fill(0, 195, 206);
    text("Prisoner", manX + manW/2 - textWidth("Prisoner")/2, manY + manH + 3*manH/20);
    text("Prisoner", manX + manW/2 - textWidth("Prisoner")/2 + scrW/2, manY + manH + 3*manH/20);
    textSize(19);
    text("1567", manX + manW/2 - textWidth("1567")/2, manY + manH + 3*manH/20 + 20);
    text("1786", manX + manW/2 - textWidth("1567")/2 + scrW/2, manY + manH + 3*manH/20 + 20);
}

// LEVEL 2 METHODS

Integer tryParse(String text) {
  try {
    return Integer.parseInt(text);
  } catch (NumberFormatException e) {
    return null;
  }
}
