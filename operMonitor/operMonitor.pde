// operMonitor of ZGame
import processing.serial.*;
//import processing.sound.*;
//SoundFile sound;
Serial arduino;
boolean recieved = false;
PrintWriter data;
boolean game;
boolean endMainGame = false;
boolean endBonusGame = false;
boolean connected;
boolean allowTouch = false;
boolean lightUp = false;
boolean sendLightCmd = false;
boolean rec = false;
int recCount = 0;
String portName;
PFont timerFont, topFont, enterFont, gadgetFont;
String alpha = "abcdefghijklmnopqrstuvwxyzABCDEFGJHIJKLMNOPQRSTUVWXYZ1234567890";
char[] teamName;
String teamNameStr;
int nameLen = 0;
float scrW = 1000;
float scrH = 0;
float gadButW, gadVoiW, gadButH;
float gadMarX, gadMarY;
float marX, marY;
float timerW, timerH, timerX, timerY;
float timerTextW;
float scoreW, scoreH, scoreX, scoreY;
float topH;
float triH, triW; // for Triangle of up and down hit count. Remove if unused...

int enterTextSize = 0;
int textSize = 0;

color green = color(0, 200, 0, 40);
color red = color(200, 0, 0);
color background = color(230);
color textCol = color(100, 2, 111, 80);
color redTextCol = color(100, 2, 50, 80);
color butCol = color(100, 2, 111, 40);
color orange = color(240, 100, 0, 40);

boolean prevMouseState = false;
boolean currMouseState = false;
boolean calculated = false;

int[] levGadCount = {3, 8, 7, 5, 5, 4};
String[] levelNames = {"START", "THUNDER", "SHIELDS", "SEALS", "UNDERGROUND", "BONUS"};
String[] gadgetNames = {"Baloon", "Press", "Gate", "Poseidon", "Trident", "Demetra-1", "Rain", "Vine", "Dionis-1", "Narcis", "Thunder", "Afina-1", "Afina-2", "Time", "Octopus", "Note", "Wind", "Ghera-1", "Fire", "Flower-1", "Flower-2", "Dionis-2", "Ghera-2", "BigKey", "Under", "Minot", "Gorgona", "Cristals", "Hercules", "Arpha", "Zodiak", "Bonus"};

byte[] passedGadgets = new byte[32];
boolean[] operPressed = new boolean[32];

int[] passedTimes = new int[32];
int sendVoiceNumber = -1;
int passTimesIndex = 0;

int[] gadgetPassedOrder = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int passOrdIndex = 0;

StopWatchTimer t;
int timerHours = 0;
int timerMinutes = 0;  
int timerSeconds = 0;
long totalSeconds = 0;
long doneTime = 0;
long lastVoiceSend = 0;
int startScores = 10000;
int currScores = 10000;
int hintDec = 500;

int hintCount = 0;
int playersCount = 0;

void setup()
{
  size(1200, 600);
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
  marX = scrW/50;
  marY = marX;
  textFont(timerFont);
  gadButW = (5*(scrW - 2*marX))/71;
  gadVoiW = (2*(scrW - 2*marX))/71;
  gadMarX = (1*(scrW - 2*marX))/71;
  gadButH = (3*(scrH - 2*marY))/24; //(6 slices by 4 parts each , 1 part - margin, 3 part - gadButH)
  gadMarY = (scrH - 2*marY)/24;

  timerW = scrW/4;
  timerH = gadButH;
  timerY = scrH - marY;

  t = new StopWatchTimer();
  totalSeconds = t.setStartTime(1, 30, 0);
  arduinoConnect();
  lastVoiceSend = totalSeconds;
  for (int g = 0; g < 32; g++)
  {
    passedGadgets[g] = 0;
    passedTimes[g] = 0;
  }
  //sound = new SoundFile(this, "sample.mp3"); // Должен лежать в папке с именем "data", папка должна лежать рядом с программой Monitor.exe
}

void draw()
{
  currMouseState = mousePressed;
  if (game)
  {
    boolean sendToBridge = false;
    background(background);
    //Timer
    strokeWeight(1);
    stroke(0);
    long elpsTime = t.getElapsedTime();
    String currTime = getTime(hours(elpsTime), minutes(elpsTime), seconds(elpsTime));
    if (t.overtime) 
    {
      currTime = "-" + currTime;
      fill(redTextCol); //color red
    }
    textFont(timerFont, timerH);
    timerTextW = textWidth(currTime);
    fill(textCol);
    text(currTime, scrW - timerTextW - marX, timerY);
    textFont(timerFont, timerH * 0.35);

    //GADGETS
    stroke(textCol);
    int gadCount = 0;

    for (int lev = 0; lev < 6; lev++)
    {
      textFont(topFont, timerH * 0.35);
      fill(textCol);
      text(levelNames[lev], marX, marY + gadMarY*(lev+1) + gadButH*lev - 2); // LEVEL NAME
      if (lev == 4 && doneTime > 0)
      {
        //print time
        String dTime = "GAME DONE AT: " + getTime(hours(doneTime), minutes(doneTime), seconds(doneTime));
        textFont(timerFont, timerH/4);
        float tTextW = textWidth(dTime);
        fill(textCol);
        text(dTime, marX + (levGadCount[lev]) * (gadMarX + gadButW + gadVoiW) - gadMarX - tTextW, marY + gadMarY*(lev+1) + gadButH*lev - 2);
        textFont(timerFont, timerH * 0.35);
      }
      for (int g = 0; g < levGadCount[lev]; g++)
      {
        if (!prevMouseState && currMouseState) // mouse click detection
        {
          if (allowTouch && mouseX > marX + g * (gadMarX + gadButW + gadVoiW) && mouseX < marX + g * (gadMarX + gadButW + gadVoiW) + gadButW && mouseY > marY + gadMarY*(lev+1) + gadButH*lev && mouseY < marY + (gadButH + gadMarY)*(lev+1))
          { 
            if (passedGadgets[gadCount] < 1) 
            {
              passedGadgets[gadCount] = 3;
              passedTimes[gadCount] = int((totalSeconds-t.getElapsedTime())/1000);
            }
            operPressed[gadCount] = true;
            sendToBridge = true;
          }
          if (allowTouch && lastVoiceSend - elpsTime > 500 && mouseX > marX + (g+1)*gadButW + (gadVoiW+gadMarX)*g && mouseX < marX + (g+1)*(gadButW+gadVoiW) + gadMarX*g  && mouseY > marY + gadMarY*(lev+1) + gadButH*lev && mouseY < marY + (gadButH + gadMarY)*(lev+1))
          {
          println("ELT="+str(elpsTime)+" lastST="+str(lastVoiceSend));
              sendVoiceNumber = gadCount;
              lastVoiceSend = elpsTime;
          }
        }
        fill(butCol);
        if (passedGadgets[gadCount] == 5) fill(green);
        else if (passedGadgets[gadCount] == 3) fill(orange);
        else fill(butCol);
        rect(marX + g * (gadMarX + gadButW + gadVoiW), marY + gadMarY*(lev+1) + gadButH*lev, gadButW, gadButH); // Level-Rects
        fill(color(20, 20, 200, 60));
        textFont(topFont, timerH * 0.24);
        float gtxtW = textWidth(gadgetNames[gadCount]);
        fill(textCol);
        text(gadgetNames[gadCount], marX + g * (gadMarX + gadButW + gadVoiW) + gadButW/2 - gtxtW/2, marY + gadMarY*(lev+1) + gadButH*(lev+0.6) - 2); // Gadget Names
        rect(marX + (g+1) * gadButW + (gadVoiW+gadMarX)*g, marY + gadMarY*(lev+1) + gadButH*lev, gadVoiW, gadButH); //Voice rect
        gadCount++;
      }
      if (lev == 4)
      {
        //Draw light button
        String lightState = lightUp? "Light is On":"Light is Off";
        if (lightUp) fill(orange);
        else fill(butCol);
        rect(scrW - (marX + gadButW + gadVoiW), marY + gadMarY*(lev+1) + gadButH*lev, gadButW+gadVoiW, gadButH); // light rect
        fill(textCol);
        float lightTextWid = textWidth(lightState);
        text(lightState, scrW - marX - (gadButW + gadVoiW)/2 - lightTextWid/2, marY + (gadMarY+gadButH)*(lev+1) - gadButH/3);
        if (!prevMouseState && currMouseState)
        {
          if (allowTouch && mouseX > scrW - (marX + gadButW + gadVoiW) && mouseX < scrW - marX && mouseY > marY + gadMarY*(lev+1) + gadButH*lev && mouseY < marY + (gadMarY+gadButH)*(lev+1))
          {
            lightUp = !lightUp;
            sendLightCmd = true;
          }
        }
      }
      strokeWeight(2);
      fill(textCol);
      line(marX, marY + gadMarY*(lev+1) + gadButH*lev, marX + (levGadCount[lev]) * (gadMarX + gadButW + gadVoiW) - gadMarX, marY + gadMarY*(lev+1) + gadButH*lev);
      strokeWeight(1);
    }

    if (allowTouch) fill(color(0, 200, 0));
    else fill(color(200, 0, 0));
    ellipse(width-25, 25, 20, 20);

    //RECIEVE FROM BRIDGE
    String fromBridge = getInput(true);

    if (fromBridge.equals("masterStart"))
    {
      for (int g = 0; g < 32; g++)
      {
        passedGadgets[g] = 0;
        operPressed[g] = false;
        sendVoiceNumber = -1;
        lastVoiceSend = 0;
      }
      t.start();
      allowTouch = true;
      println("Run game");
    } else if (fromBridge.equals("masterConnected"))
    {
      t = new StopWatchTimer();
      totalSeconds = t.setStartTime(1, 10, 0);
      println("Connecting to Master: true");
      for (int g = 0; g < 32; g++) 
      {
        passedGadgets[g] = 0;
        operPressed[g] = false;
      }
    } else if (fromBridge.startsWith("BD") && fromBridge.endsWith("FF"))
    {
      println(fromBridge);
      for (int i = 0; i < fromBridge.length()-4; i++)
      {
        //print(fromBridge.charAt(i));
        int data = Integer.parseInt(String.valueOf(fromBridge.charAt(i+2)));
        if (data == 5)
        {
          passedGadgets[i] = 5;
          print("Gadget ");
          print(i);
          println(" done by player");
          passedTimes[i] = int((totalSeconds-t.getElapsedTime())/1000);
        }
      }
    }
    //SEND TO BRIDGE
    if (sendToBridge)
    {
      print("...sending to bridge...");
      arduino.write("CD");
      for (int s = 0; s < 32; s++)
      {
        arduino.write(operPressed[s]?"3":"1");
        operPressed[s] = false;
      }
      arduino.write("FF\n");
      println("OK");
      sendToBridge = false;
    }

    if (sendVoiceNumber >= 0)
    {
      print("sending voice hint...");
      arduino.write("CA");
      arduino.write(str(sendVoiceNumber+1));
      arduino.write("FF");
      println("OK");
      sendVoiceNumber = -1;
    }

    if (sendLightCmd)
    {
      print("sending voice hint...");
      arduino.write("CF");
      if (lightUp) arduino.write("LU"); 
      else arduino.write("LD");
      arduino.write("FF");
      println("OK");
      sendLightCmd = false;
    }
    boolean mainLevelsDone = true;
    for (int g = 0; g < gadCount-4; g++)
    {
      if (passedGadgets[g] == 0) mainLevelsDone = false;
    }

    if (mainLevelsDone && !endMainGame) 
    {
      doneTime = elpsTime;
      endMainGame = true;
    }
    
    if (endMainGame && !endBonusGame)
    {
      boolean fullLevelsDone = true;
      for (int g = gadCount-4; g < gadCount; g++)
      {
        if (passedGadgets[g] == 0) fullLevelsDone = false;
      }
      if (fullLevelsDone) 
      {
        endBonusGame = true;
        t.stop();
      }
    }
    //allowTouch = false;
    //if(t.running) t.stop();
    //sound.play();
    /*
      fill(textCol);
     stroke(0);
     rect(scrW/2 - (gadButW+gadVoiW)/2, scrH - gadButH - marY, gadButW+gadVoiW, gadButH);
     float restartTextWid = textWidth("RESTART");
     fill(color(10, 10, 200));
     text("RESTART", scrW/2 - restartTextWid/2, scrH - marY - 10);
     */
  } else // Enter params to enter the game
  {
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
  if (h < 10) h_str = "0" + str(h);
  String m_str = str(m);
  if (m < 10) m_str = "0" + str(m);
  String s_str = str(s);
  if (s < 10) s_str = "0" + str(s);
  return h_str+":"+m_str+":"+s_str;
}

int seconds(long elpTime) {
  return int((elpTime / 1000) % 60);
}
int minutes(long elpTime) {
  return int((elpTime / (1000*60)) % 60);
}
int hours(long elpTime) {
  return int((elpTime / (1000*60*60)) % 24);
}

String getInput(boolean debug)
{
  if (arduino.available() > 0)
  {
    //println("start read string from serial::"+str(millis()));
    String inp = arduino.readStringUntil('\n');
    //println("end read string from serial::"+str(millis()));
    if (inp != null)
    {
      if (inp.length() > 1 && debug) println(inp);
      inp = inp.trim();
      //println("print string from serial::"+str(millis()));
      return inp;
    } else return " ";
  } else return " ";
}
