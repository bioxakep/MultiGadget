void endGame()
{      
  background(0);
  fill(200);
  strokeWeight(2);
  fill(0);
  stroke(255);
  ellipse(scrW/2 - 122, scrH/2 - 51, 40, 40); // minus circle
  ellipse(scrW/2 + 118, scrH/2 - 51, 40, 40); // minus circle
  fill(200);
  strokeWeight(0);
  rect(scrW/2 - 135, scrH/2 - 52, 28, 4); // minus

  rect(scrW/2 + 105, scrH/2 - 52, 28, 4); // plus horiz
  rect(scrW/2 + 117, scrH/2 - 64, 4, 28); // plus vertical
  enterTextSize = calcTSize(str(playersCount) + " PLAYERS", 190);
  textFont(enterFont, enterTextSize);
  float playersTextWidth = textWidth(str(playersCount) + " PLAYERS");
  text(str(playersCount) + " PLAYERS", scrW/2 - playersTextWidth/2, scrH/2 - 50 + enterTextSize/3);

  teamNameStr = new String(teamName);
  teamNameStr = teamNameStr.substring(0, nameLen);
  float nameTextWidth = textWidth("TEAM NAME: " + teamNameStr);
  text("TEAM NAME: "+ teamNameStr, scrW/2 - nameTextWidth/2, scrH/2 + enterTextSize/3);

  enterTextSize = calcTSize("PLAY", 90);
  textFont(enterFont, enterTextSize);
  fill(255);
  if (playersCount > 0) text("PLAY", scrW/2 - 40, scrH/2 + 60);

  if (!prevMouseState && currMouseState)
  {
    if (mouseX > scrW/2 - 140 && mouseX < scrW/2 - 100 && mouseY > scrH/2 - 70 && mouseY < scrH/2 - 40)
    {
      playersCount--;
      if (playersCount < 0) playersCount = 0;
    }
    if (mouseX > scrW/2 + 100 && mouseX < scrW/2 + 135 && mouseY > scrH/2 - 70 && mouseY < scrH/2 - 40)
    {
      playersCount++;
    }
    //Play button 
    if (mouseX > scrW/2 - 45 && mouseX < scrW/2 - 40 + playersTextWidth && mouseY > scrH/2 + 63 - enterTextSize && mouseY < scrH/2 + 63 && playersCount > 0)
    {
      
    }
  }
}
