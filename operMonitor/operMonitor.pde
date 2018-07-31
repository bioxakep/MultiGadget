import processing.serial.*;
Serial arduino;
boolean game;
boolean connected;
String portName;
PFont timerFont, enterFont, panelFont;
float scrH, scrW;
float panelW, panelH;
float panelItemW, panelItemH;
float panelxMar, panelyMar;
float triH, triW;
float timerW, timerX, timerY;
float cntrlW, cntrlH; 
float tMaxWidth;
float vertOffset = 10;
int textSize = 0;
color green = color(0, 200, 0);
color red = color(200, 0, 0);
color background = color(230);
color text = color(100, 2, 111, 30);
boolean prevMouseState = false;
boolean currMouseState = false;
boolean[] inputGadgetStates = {false,false,false,false,false,false,false,false,false,false,false,false};
boolean[] currGadgetStates = {false,false,false,false,false,false,false,false,false,false,false,false};
boolean[] gadgetHinted = {false,false,false,false,false,false,false,false,false,false,false,false};
long[] gadgetPassedTimes = {0,0,0,0,0,0,0,0,0,0,0,0};
int[] gadgetPassedOrder = {0,0,0,0,0,0,0,0,0,0,0,0};
boolean useHint = false;
StopWatchTimer t;
long totalSeconds = 0;
int startScores = 10000;
int hintDec = 500;
int hintCount = 0;
int checkSum = 0;
int playersCount = 0;

int timerHours = 0;
int timerMinutes = 0;
int timerSeconds = 0;

void setup()
{
  size(1000, 700);
  //fullScreen();
  fill(255);
  
  game = false;//DEBUG
  timerFont = createFont("Silom", 14);
  enterFont = createFont("MyanmarMN", 14);
  panelFont = createFont("Silom", 16);
  scrW = width;
  scrH = height;
  panelH = 0.9 * scrH;
  panelW = scrW - (scrH - panelH)/2;
  float yK = 7;
  float xK = 8;
  panelxMar = panelW/(xK*3 + 2);
  panelyMar = panelH/(yK*5 + 4);
  panelItemH = panelyMar*yK;
  panelItemW = panelxMar*xK;
  triH = panelItemH/3;
  tMaxWidth = panelW - panelItemW*4/3 - 2*panelxMar;
  t = new StopWatchTimer();
  t.setStartTime(0,1,30);
  totalSeconds = 0 + 1 * 60 + 30;
  arduinoConnect();
}
void draw()
{
  currMouseState = mousePressed;
  if (game)
  {
    String data = getInput();
    if(data.length() > 10)
    {
      //println(data);
      
      for(int d = 0; d < 12; d++)
      {
        String currState = data.substring(d,d+1);
        //println("state "+ str(d) + ":" + currState);
        if(currState.equals("1")) inputGadgetStates[d] = true;
        else inputGadgetStates[d] = false;
        //println("input:"+str(inputGadgetStates[d]) + ",current:"+str(currGadgetStates[d]));
        if(inputGadgetStates[d] && !currGadgetStates[d])
        {
          gadgetPassedOrder[checkSum] = d;
          checkSum++;
          println("Gadget "+str(d+1) + " passed");
          gadgetPassedTimes[d] = t.passedTime();
          currGadgetStates[d] = inputGadgetStates[d];
          if(useHint) 
          {
            gadgetHinted[d] = true;
            useHint = false;
          }
        }
      }
      if(checkSum == 12)
      {
        for(int z = 0; z < 12; z++)
        {
          String end = ".";
          if(gadgetHinted[z]) end = " with hint.";
          println("Gadget "+str(gadgetPassedOrder[z])+" solved at "+str(gadgetPassedTimes[gadgetPassedOrder[z]]) + end);
          noLoop();
        }
      }
    }
    background(background);
    fill(background);
    // hintCountRect
    strokeWeight(1);
    rect(scrW/2 - panelW/2, scrH/2 - panelH/2, 2*panelItemW/3, panelItemH); //hintRect
    rect(scrW/2 + panelW/2 - 2*panelItemW/3, scrH/2 - panelH/2, 2*panelItemW/3, panelItemH);
    //inrects + - 
    strokeWeight(0);
    fill(text);
    float cTextSize = calcTSize("PLAYERS IN GAME", 4 * panelItemW / 9);
    textFont(panelFont, cTextSize);
    float lefttextWidth = textWidth("HINT COUNT");
    text("HINT COUNT", scrW/2 - panelW/2 + panelItemW / 3 - lefttextWidth / 2, scrH/2 - panelH/2 + cTextSize);
    text("PLAYERS IN GAME", scrW/2 + panelW/2 -2*panelItemW/3 + panelItemW/9, scrH/2 - panelH/2 + cTextSize); 
    
    float xTimerPos = scrW/2 - panelW/2 + 2*panelItemW/3 + panelxMar;
    float yTimerPos = scrH/2 - panelH/2 + panelItemH - textSize/3;
    String currTime = getTime(t.hour(), t.minute(), t.second());
    textSize = calcTSize(currTime, tMaxWidth);
    textFont(timerFont, textSize);
    text(currTime, xTimerPos, yTimerPos);
    textSize = calcTSize(currTime, tMaxWidth/2);
    textFont(timerFont, textSize);
    float txtWidth = textWidth(str(playersCount));
    text(str(playersCount), scrW/2 + panelW/2 -2*panelItemW/3 + 2*panelItemW/6 - txtWidth/2, scrH/2 - panelH/2 + panelItemH/2 + triH/2);
    txtWidth = textWidth(str(hintCount));
    text(str(hintCount), scrW/2 - panelW/2 + 2*panelItemW/6 - txtWidth/2, scrH/2 - panelH/2 + panelItemH/2 + triH/2);
    if(!prevMouseState && currMouseState) //hint++
    {
      //hint minus
      if(mouseX > scrW/2 - panelW/2 && mouseX < scrW/2 - panelW/2 + 2*panelItemW/9 && mouseY > scrH/2 - panelH/2 && mouseY < scrH/2 - panelH/2 + panelItemH)
      {
        hintCount--;
        if(hintCount < 0) hintCount = 0;
      }
      if(mouseX > scrW/2 - panelW/2 + 4*panelItemW/9 && mouseX < scrW/2 - panelW/2 + 2*panelItemW/3 && mouseY > scrH/2 - panelH/2 && mouseY < scrH/2 - panelH/2 + panelItemH)
      {
        hintCount++;
        useHint = true;
      }
      //if(mouseX > scrW/2 - 140 && mouseX < scrW/2 - 100 && mouseY > scrH/2 - 70 && mouseY < scrH/2 - 40)
    }
    
    fill(background);
    //rect(scrW/2 - panelW/2, scrH/2 - panelH/2 + 1, 2*panelItemW/9, panelItemH - 2); //-
    triangle(scrW/2 - panelW/2 + 2*panelItemW/54, scrH/2 - panelH/2 + panelItemH/2 - triH/2,
             scrW/2 - panelW/2 + 2*5*panelItemW/54, scrH/2 - panelH/2 + panelItemH/2 - triH/2,
             scrW/2 - panelW/2 + panelItemW/9, scrH/2 - panelH/2 + panelItemH/2 + triH/2); 
    //rect(scrW/2 - panelW/2 + 4*panelItemW/9, scrH/2 - panelH/2 + 1, 2*panelItemW/9, panelItemH - 2); //+
    triangle(scrW/2 - panelW/2 + 26*panelItemW/54, scrH/2 - panelH/2 + panelItemH/2 + triH/2,
             scrW/2 - panelW/2 + 34*panelItemW/54, scrH/2 - panelH/2 + panelItemH/2 + triH/2,
             scrW/2 - panelW/2 + 5*panelItemW/9, scrH/2 - panelH/2 + panelItemH/2 - triH/2); 
    strokeWeight(1);
    // gadgets rects
    for (int i = 0; i < 12; i++) 
    {
      if(currGadgetStates[i]) fill(green);
      else fill(background);
      rect(scrW/2 - panelW/2 + (i%3)*(panelItemW+panelxMar), scrH/2 - panelH/2 + (panelItemH + panelyMar)*(i/3 + 1), panelItemW, panelItemH);
      txtWidth = textWidth(str(i+1));
      fill(text);
      text(str(i+1),scrW/2 - panelW/2 + (i%3)*(panelItemW+panelxMar) + panelItemW/2 - txtWidth/2, scrH/2 - panelH/2 + (panelItemH + panelyMar)*(i/3 + 1) + panelItemH - (panelItemH-textSize)/2);
    }
  }
  else
  {
    background(0);
    float cTextSize = calcTSize("PLAY", 90);
    textFont(enterFont, cTextSize);
    fill(255);
    if(playersCount > 0) text("PLAY",scrW/2 - 40, scrH/2 + 60);
    fill(200);
    strokeWeight(0);
    rect(scrW/2 - 135, scrH/2 - 52, 28, 4); // minus
    rect(scrW/2 + 105, scrH/2 - 52, 28, 4); // plus horiz
    rect(scrW/2 + 117, scrH/2 - 64, 4, 28); // plus vertical
    cTextSize = calcTSize(str(playersCount) + " players", 190);
    textFont(enterFont, cTextSize);
    float cTextWidth = textWidth(str(playersCount) + " players");
    text(str(playersCount) + " players", scrW/2 - cTextWidth/2, scrH/2 - 50 + cTextSize/3);
    
    if(!prevMouseState && currMouseState)
    {
      if(mouseX > scrW/2 - 140 && mouseX < scrW/2 - 100 && mouseY > scrH/2 - 70 && mouseY < scrH/2 - 40)
      {
        playersCount--;
        if(playersCount < 0) playersCount = 0;
      }
      if(mouseX > scrW/2 + 100 && mouseX < scrW/2 + 135 && mouseY > scrH/2 - 70 && mouseY < scrH/2 - 40)
      {
        playersCount++;
      }
      //Play button 
      if(mouseX > scrW/2 - 45 && mouseX < scrW/2 - 40 + cTextWidth && mouseY > scrH/2 + 63 - cTextSize && mouseY < scrH/2 + 63 && playersCount > 0)
      {
        game = true;
        strokeWeight(1);
        stroke(0);
        t.start();
      }
    }
  }
  prevMouseState = currMouseState;
}

void arduinoConnect()
{
  connected = false;
  portName = "COM3"; // COM3 or /dev/tty.wchusbserial1410 or /dev/tty.wchusbserial1420
  //portName = "/dev/tty.wchusbserial1411";
  portName = "/dev/cu.usbmodem1411";
  arduino = new Serial(this, portName, 9600);
  long startConnect = millis();
  while (!connected && (millis() - startConnect < 60000))
  {
    arduino.write("letsgame\n");
    println("Connecting...");
    long now = millis();
    while (millis() - now < 1000) {
      ;
    }
    String input = getInput(); 
    if (input.length() > 4)
    {
      if (input.trim().equals("start")) connected = true;
    }
  }
  println("Connect status: " + str(connected));
  if (!connected) { 
    noLoop(); 
    println("No connection for arduino..."); 
    text("NO ARDUINO CONNECTED TO " + portName, width/2, height/2);
  }
}

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

int calcTSize(String txt, float maxWidth)
{
  int retSize = 1;
  float calcWidth = 0;
  while (calcWidth < maxWidth)
  {
    if (txt.equals("")) txt = "00:00:00";
    timerFont = createFont("Silom", 14);
    textFont(timerFont, retSize);
    calcWidth = textWidth(txt);
    //println(retSize+":"+txt+":"+calcWidth);
    retSize += 1;
  }
  return --retSize;
}

String getTime(int h, int m, int s)
{
  String h_str = h > 9 ? str(h) : "0" + str(h);
  String m_str = m > 9 ? str(m) : "0" + str(m);
  String s_str = s > 9 ? str(s) : "0" + str(s);
  return h_str+":"+m_str+":"+s_str;
}