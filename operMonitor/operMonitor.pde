import processing.serial.*;
Serial arduino;
boolean recieved = false;

PrintWriter data;

boolean game;
boolean endGame = false;
boolean connected;
String portName;
PFont timerFont, topFont, enterFont, gadgetFont;
String alpha = "abcdefghijklmnopqrstuvwxyzABCDEFGJHIJKLMNOPQRSTUVWXYZ1234567890";
char[] teamName;
String teamNameStr;
int nameLen = 0;
float scrW = 1000;
float scrH = 0;
float gadButW, gadButH;
float gadMarX, gadMarY;
float timerW, timerH, timerX, timerY;
float timerTextW;
float scoreW, scoreH, scoreX, scoreY;
float topH;
float triH, triW; // for Triangle of up and down hit count. Remove if unused...

int enterTextSize = 0;
int textSize = 0;

color green = color(0, 200, 0);
color red = color(200, 0, 0);
color background = color(230);
color textCol = color(100, 2, 111, 80);
color butCol = color(100, 2, 111, 40);

boolean prevMouseState = false;
boolean currMouseState = false;
boolean calculated = false;

int[] levGadCount = {3, 9, 7, 6, 5, 1};
String[] levelNames = {"START", "THUNDER", "SHIELDS", "SEALS", "UNDERGROUND", "END"};
String[] gadgetNames = {"Baloon", "Press", "Gate", "Poseidon", "Trident", "Demetra-1", "Rain", "Vine", "Dionis-1", "Hercules", "Narcis", "Thunder", "Afina-1", "Afina-2", "Time", "Octopus", "Note", "Wind", "Ghera-1", "Fire", "Flower-1", "Flower-2", "Arpha", "Dionis-2", "Ghera-2", "Zodiak", "Minot", "Gorgona", "Cristals", "Light", "End"};
boolean[] passedGadgets = new boolean[31];
boolean[] hintedGadgets = new boolean[31];
int[] passedTimes = new int[31];
byte[] inData = new byte[31];
int passTimesIndex = 0;

int[] gadgetPassedOrder = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int passOrdIndex = 0;

StopWatchTimer t;
int timerHours = 0;
int timerMinutes = 0;  
int timerSeconds = 0;
long totalSeconds = 0;

int startScores = 10000;
int currScores = 10000;
int hintDec = 500;

int hintCount = 0;
int playersCount = 0;

void setup()
{
  size(1200, 675);
  //fullScreen();
  fill(255);
  teamName = new char[15];
  game = true;//DEBUG
  timerFont = createFont("Arial", 10);//Silom
  topFont = createFont("MyanmarMN", 14);
  enterFont = createFont("MyanmarMN", 14);
  gadgetFont = createFont("Silom", 16);
  scrW = width;
  scrH = height;
  topH = scrH/9;
  textFont(timerFont);
  timerW = scrW/4;
  timerH = topH * 0.7;//calcTSize("00:00:00", timerW);
  timerY = topH * 0.85;
  scoreW = scrW/3;
  scoreH = timerH;
  scoreY = timerY;

  gadMarX = scrW/55;
  gadMarY = (scrH-topH)/24;
  gadButW = gadMarX*5-5;
  gadButH = gadMarY*3-2;

  t = new StopWatchTimer();
  t.setStartTime(0, 1, 30);
  totalSeconds = 0 + 1 * 60 + 30;
  arduinoConnect();
  for (int c = 0; c < 15; c++) teamName[c] = ' ';
  for (int g = 0; g < 31; g++)
  {
    passedGadgets[g] = false;
    hintedGadgets[g] = false;
    passedTimes[g] = 0;
    inData[g] = 0;
  }
}

void draw()
{
  currMouseState = mousePressed;
  if (game)
  {
    background(background);
    //Timer
    strokeWeight(1);
    stroke(0);
    String currTime = getTime(t.hour(), t.minute(), t.second());
    textFont(timerFont, timerH);
    timerTextW = textWidth(currTime);
    fill(textCol);
    text(currTime, 0, timerY);
    float scoreTextW = textWidth(str(currScores));
    text(currScores, scrW - scoreTextW, timerY);

    textFont(timerFont, timerH * 0.35);
    text("ELAPSED TIME", 5, timerH * 0.35);
    float scoresTextW = textWidth("SCORES");
    text("SCORES", scrW - scoresTextW - 5, timerH * 0.35);

    float playersTextW = textWidth("PLAYERS: "+ str(playersCount) + " TEAM:" + teamNameStr);
    text("PLAYERS: "+ str(playersCount) + " TEAM:" + teamNameStr, scrW/2 - playersTextW/2, timerH * 0.35);

    // HINT DRAW
    textFont(timerFont, timerH);
    float hintTextW = textWidth("HINTS: "+ str(hintCount));
    text("HINTS: "+ str(hintCount), scrW/2 - hintTextW/2, timerY);
    fill(butCol);
    strokeWeight(0);
    ellipse(scrW/2 - hintTextW/2 - 30, timerY - timerH/2 + 6, 50, 50); // minus circle
    ellipse(scrW/2 + hintTextW/2 + 30, timerY - timerH/2 + 6, 50, 50); // plus circle

    fill(background);
    rect(scrW/2 - hintTextW/2 - 48, timerY - timerH/2 + 4, 36, 4); // minus for minus
    rect(scrW/2 + hintTextW/2 + 12, timerY - timerH/2 + 4, 36, 4); // minus for plus
    rect(scrW/2 + hintTextW/2 + 28, timerY - timerH/2 - 12, 4, 36); // plus vertical

    // Hint Inc and Dec Mouse Click
    if (!prevMouseState && currMouseState) //hint + -
    {
      //hint minus
      if (mouseX > (scrW/2 - hintTextW/2 - 55) && mouseX < scrW/2 - hintTextW/2 - 5  && mouseY > timerY - timerH/2 - 44 && mouseY < timerY - timerH/2 + 56)
      {
        hintCount--;
        if (hintCount < 0) hintCount = 0;
      }
      if (mouseX > (scrW/2 + hintTextW/2 + 5) && mouseX < scrW/2 + hintTextW/2 + 55  && mouseY > timerY - timerH/2 - 44 && mouseY < timerY - timerH/2 + 56)
      {
        hintCount++;
      }
    }

    //GADGETS
    stroke(textCol);
    int gadCount = 0;

    for (int lev = 0; lev < 6; lev++)
    {
      textFont(topFont, timerH * 0.35);
      fill(textCol);
      text(levelNames[lev], gadMarX, topH + gadMarY*(lev+1) + gadButH*lev - 2);
      for (int g = 0; g < levGadCount[lev]; g++)
      {
        if (!prevMouseState && currMouseState)
        {
          if (mouseX > ((g+1) * gadMarX + gadButW * g) && mouseX < ((g+1) * (gadMarX + gadButW))  && mouseY > (topH + gadMarY * (lev+1) + gadButH*lev) && mouseY < (topH + (lev+1) * (gadMarY + gadButH)))
          { 
            hintedGadgets[gadCount] = true;
            passedTimes[gadCount] = getSecs(t.hour(), t.minute(), t.second());
          }
        }
        fill(butCol);
        if (passedGadgets[gadCount]) fill(color(0, 200, 100, 60));
        else fill(color(100, 100, 100, 60));
        if (hintedGadgets[gadCount]) fill(color(255, 180, 130, 60));
        rect((g+1) * gadMarX + gadButW * g, topH + gadMarY*(lev+1) + gadButH*lev, gadButW, gadButH);
        textFont(topFont, timerH * 0.3);
        float gtxtW = textWidth(gadgetNames[gadCount]);
        fill(textCol);
        text(gadgetNames[gadCount], (g+1) * gadMarX + gadButW * g + gadButW/2 - gtxtW/2, topH + gadMarY*(lev+1) + gadButH*(lev+0.6) - 2);
        gadCount++;
      }
      strokeWeight(2);
      fill(textCol);
      line(gadMarX, topH + gadMarY*(lev+1) + gadButH*lev, (levGadCount[lev]) * (gadMarX + gadButW), topH + gadMarY*(lev+1) + gadButH*lev);
      strokeWeight(1);
    }

    //RECIEVE FROM BRIDGE
    if (arduino.available() > 0)
    {
      boolean start = true;
      byte input = (byte)(0xFF & (arduino.read()));
      //println(input);
      //println(hex(input));
      if (hex(input).equals("BB"))
      {
        for (int i = 0; i < 31; i++)
        {
          byte inByte = (byte)(0xFF & arduino.read());
          inData[i] = inByte;
        }
        byte last = (byte)(0xFF & arduino.read());
        if (hex(last).equals("FF")) recieved = true;
      }
      else if (hex(input).equals("EE"))
      {
        t.start(); 
        println("start game");
      }
      else arduino.readStringUntil('\n');
    }
    if (recieved)
    {
      print("Recieved from bridge:");
      
      for (int p = 0; p < 31; p++)
      {
        print(inData[p]);
        print("|");
        if (inData[p] > 3 && !passedGadgets[p]) passedGadgets[p] = true;
        inData[p] = 0;
      }
      println();
      recieved = false;
      
    }

    //SEND TO BRIDGE
    boolean sendToBridge = false;
    for (int i = 0; i < 31; i++)
    {
      if (hintedGadgets[i] && !passedGadgets[i])
      {
        sendToBridge = true;
        passedGadgets[i] = true;
      }
    }

    if (sendToBridge)
    {
      print("sending to bridge hints..");
      arduino.write(0xCC);
      for (int s = 0; s < 31; s++)
      {
        byte outByte = 0x01;
        if (hintedGadgets[s]) outByte = 0x05;
        print(int(hintedGadgets[s]));
        print("-");
        arduino.write(outByte);
      }
      arduino.write(0xFF);
      println("OK");
      sendToBridge = false;
    }

    boolean allDone = true;
    for (int g = 0; g < gadCount; g++)
    {
      if (!hintedGadgets[g] && !passedGadgets[g]) allDone = false;
    }
    if (allDone) 
    {
      game = false;
      endGame = true;
    }
  } else // Enter params to enter the game
  {
    if (endGame)
    {
      background(0);
      fill(color(200, 200, 200));
      textFont(topFont, scrH/20);
      float statTextWidth = textWidth("STATISTICS");
      text("STATISTICS", scrW/2 - statTextWidth/2, scrH/19);
      textFont(topFont, 17*scrH/(32*18));
      data = createWriter("data.txt");
      data.println("Date: "+str(year()) +"."+str(month())+"."+str(day()));
      data.println("Time: "+str(hour()) +":"+str(minute())+":"+str(second()));
      data.println("Team Name: " + teamNameStr);
      data.println("Players: " + str(playersCount));
      for (int g = 0; g < passedTimes.length; g++)
      {
        String result = str(g+1) + ". " + gadgetNames[g] + " passed by " + passedTimes[g] + " seconds";
        if (hintedGadgets[g]) result = result + " and hinted.";
        else result = result + ".";
        String csvString = g+";"+gadgetNames[g]+";"+passedTimes[g]+";"+hintedGadgets[g]+";";
        data.println(csvString);
        text(result, scrW/2 - textWidth(result)/2, scrH/19+(g+1)*17*scrH/(32*18));
      }
      data.flush();
      data.close();
      if (!prevMouseState && currMouseState) 
      {
        endGame = false;
        for (int c = 0; c < 15; c++) teamName[c] = ' ';
        for (int g = 0; g < 31; g++)
        {
          passedGadgets[g] = false;
          hintedGadgets[g] = false;
          passedTimes[g] = 0;
          inData[g] = 0;
        }
        arduino.write(0xCF);
        //RESET GAME
      }
    } else endGame();
  }
  prevMouseState = currMouseState;
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
  String h_str = str(h);
  if (h < 10) h_str= "0" + str(h);
  //String m_str = m > 9 ? str(m) : "0" + str(m);
  String m_str = str(m);
  if (m < 10) m_str= "0" + str(m);
  //String s_str = s > 9 ? str(s) : "0" + str(s);
  String s_str = str(s);
  if (s < 10) s_str= "0" + str(s);
  if ( s == 1 && m == 0 && h == 0) t.stop();
  //print(h_str+":"+m_str+":"+s_str);
  return h_str+":"+m_str+":"+s_str;
}

Integer getSecs(int h, int m, int s)
{
  return (h*3600 + m*60 + s);
}

void keyPressed()
{
  if (keyCode == 27) key = 0;
  int index = alpha.indexOf(key);
  if (index >=0 && keyCode != 8 && nameLen < 15)
  {
    teamName[nameLen] = key;
    nameLen++;
    println(teamName);
  }
  if (keyCode == 8 && nameLen > 0)
  {
    nameLen--;
    teamName[nameLen] = ' ';
    println(teamName);
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

void arduinoConnect()
{
  connected = false;
  portName = "COM18"; // COM3 or /dev/tty.wchusbserial1410 or /dev/tty.wchusbserial1420

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
    println(input);
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
