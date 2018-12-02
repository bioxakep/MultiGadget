// 22.10.2018 version
import processing.serial.*;
Serial arduino;
boolean recieved = false;

PrintWriter data;

boolean game;
boolean endGame = false;
boolean connected;
boolean allowTouch = true;
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

color green = color(0, 200, 0);
color red = color(200, 0, 0);
color background = color(230);
color textCol = color(100, 2, 111, 80);
color butCol = color(100, 2, 111, 40);

boolean prevMouseState = false;
boolean currMouseState = false;
boolean calculated = false;

int[] levGadCount = {3, 9, 7, 6, 6, 1};
String[] levelNames = {"START", "THUNDER", "SHIELDS", "SEALS", "UNDERGROUND", "END"};
String[] gadgetNames = {"Baloon", "Press", "Gate", "Poseidon", "Trident", "Demetra-1", "Rain", "Vine", "Dionis-1", "Hercules", "Narcis", "Thunder", "Afina-1", "Afina-2", "Time", "Octopus", "Note", "Wind", "Ghera-1", "Fire", "Flower-1", "Flower-2", "Arpha", "Dionis-2", "Ghera-2", "BigKey", "Under", "Zodiak", "Minot", "Gorgona", "Cristals", "End"};
boolean[] passedGadgets = new boolean[32];
boolean[] hintedGadgets = new boolean[32];
int[] passedTimes = new int[32];
//int[] gadVoiceHintNum = new int[32];
int sendVoiceNumber = -1;
int passTimesIndex = 0;

int[] gadgetPassedOrder = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int passOrdIndex = 0;

StopWatchTimer t;
int timerHours = 0;
int timerMinutes = 0;  
int timerSeconds = 0;
long totalSeconds = 0;
long lastVoiceSend = 0;
int startScores = 10000;
int currScores = 10000;
int hintDec = 500;

int hintCount = 0;
int playersCount = 0;

// draw
//LEVELS
//-[]-[]-[]-
//-[]-[]-[]-[]-[]-[]-[]-[]-[]-
//-[]-[]-[]-[]-[]-[]-[]-
//-[]-[]-[]-[]-[]-[]-
//-[]-[]-[]-[]-[]-[]-
//-[]-                   TIME

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
  //topH = scrH/9;
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
  //arduinoConnect();
  lastVoiceSend = totalSeconds;
  for (int g = 0; g < 32; g++)
  {
    passedGadgets[g] = false;
    hintedGadgets[g] = false;
    //gadVoiceHintNum[g] = 0;
    passedTimes[g] = 0;
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
      for (int g = 0; g < levGadCount[lev]; g++)
      {
        if (!prevMouseState && currMouseState) // mouse click detection
        {
          if (allowTouch && !hintedGadgets[gadCount] && !passedGadgets[gadCount] && mouseX > marX + g * (gadMarX + gadButW + gadVoiW) && mouseX < marX + g * (gadMarX + gadButW + gadVoiW) + gadButW && mouseY > marY + gadMarY*(lev+1) + gadButH*lev && mouseY < marY + (gadButH + gadMarY)*(lev+1))
          { 
            hintedGadgets[gadCount] = true;
            passedTimes[gadCount] = int((totalSeconds-t.getElapsedTime())/1000);
            println("mouse touch button "+str(gadCount) + " MX="+str(mouseX)+" and MY="+str(mouseY));
          }
          if (allowTouch && mouseX > marX + (g+1)*gadButW + (gadVoiW+gadMarX)*g && mouseX < marX + (g+1)*(gadButW+gadVoiW) + gadMarX*g  && mouseY > marY + gadMarY*(lev+1) + gadButH*lev && mouseY < marY + (gadButH + gadMarY)*(lev+1))
          {
            long elTime = t.getElapsedTime();
            println("ELT="+str(elTime)+" lastST="+str(lastVoiceSend));
            if (lastVoiceSend - elTime > 500)
            {
              sendVoiceNumber = gadCount;
              lastVoiceSend = elTime;
            }
          }
        }
        fill(butCol);
        if (passedGadgets[gadCount]) fill(color(0, 200, 100, 60));
        else fill(color(100, 100, 100, 60));
        if (hintedGadgets[gadCount]) fill(color(255, 180, 130, 60));
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
        if (lightUp) fill(color(10, 10, 150));
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

    //RECIEVE FROM BRIDGE
    //String fromBridge = getInput(true);
    
    if (fromBridge.equals("masterStart"))
     {
     for (int g = 0; g < 32; g++)
     {
     passedGadgets[g] = false;
     hintedGadgets[g] = false;
     }
     t.start();
     allowTouch = true;
     println("Run game");
     } else if (fromBridge.equals("masterConnected"))
     {
     t = new StopWatchTimer();
     totalSeconds = t.setStartTime(1, 30, 0);
     println("Connecting to Master: true");
     for (int g = 0; g < 32; g++)
     {
     passedGadgets[g] = false;
     hintedGadgets[g] = false;
     }
     } else if (fromBridge.startsWith("BB") && fromBridge.endsWith("FF"))
     {
     println(fromBridge);
     for (int i = 0; i < fromBridge.length()-4; i++)
     {
     //print(fromBridge.charAt(i));
     int data = Integer.parseInt(String.valueOf(fromBridge.charAt(i+2)));
     if (data > 3)
     {
     passedGadgets[i] = true;
     print("Gadget "); 
     print(i);
     println(" passed by player");
     passedTimes[i] = int((totalSeconds-t.getElapsedTime())/1000);
     }
     }
     }
     
    //SEND TO BRIDGE
    boolean sendToBridge = false;
    for (int i = 0; i < 32; i++)
    {
      if (hintedGadgets[i] && !passedGadgets[i])
      {
        //print("gadget"); print(i+1); println(" hinted, sending...");
        sendToBridge = true;
        passedGadgets[i] = true;
        print("Hint "); 
        print(i+1); 
        print(" Gadget");
        //print("Hinted state:"); print(hintedGadgets[i]); print(" Passed:"); println(passedGadgets[i]);
      }
    }

    if (sendToBridge)
    {
      print("...sending to bridge...");
      arduino.write("CC");
      for (int s = 0; s < 32; s++)
      {
        arduino.write(passedGadgets[s]?"5":"1");
      }
      arduino.write("FF\n");
      println("OK");
      sendToBridge = false;
    }

    if (sendVoiceNumber >= 0)
    {
      print("sending voice hint...");
      arduino.write("CD");
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
      ;
    }

    boolean allDone = true;
    for (int g = 0; g < gadCount; g++)
    {
      if (!hintedGadgets[g] && !passedGadgets[g]) allDone = false;
    }

    if (allDone) 
    {
      allowTouch = false;
      fill(textCol);
      stroke(0);
      rect(scrW/2 - (gadButW+gadVoiW)/2, scrH - gadButH - marY, gadButW+gadVoiW, gadButH);
      float restartTextWid = textWidth("RESTART");
      fill(color(10, 10, 200));
      text("RESTART", scrW/2 - restartTextWid/2, scrH - marY - 10);
      if (!prevMouseState && currMouseState)
      {
        if (allowTouch && mouseX > scrW/2 - (gadButW+gadVoiW)/2 && mouseX < scrW/2 + (gadButW+gadVoiW)/2 && mouseY > scrH - gadButH - marY && mouseY < scrH - marY)
        {
          allDone = false;
          for (int c = 0; c < 15; c++) teamName[c] = ' ';
          for (int g = 0; g < 32; g++)
          {
            passedGadgets[g] = false;
            hintedGadgets[g] = false;
            passedTimes[g] = 0;
            lightUp = false;
          }
          t = new StopWatchTimer();
          totalSeconds = t.setStartTime(1, 30, 0);
          lastVoiceSend = totalSeconds;
          arduino.write("ClearStates");//rewrite
          //RESET GAME
        }
      }
    }
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

void arduinoConnect()
{
  connected = false;
  print("Connecting to Bridge: ");
  portName = "COM18"; // COM3 or /dev/tty.wchusbserial1410 or /dev/tty.wchusbserial1420
  arduino = new Serial(this, portName, 9600);
  long startConnect = millis();
  while (!connected && (millis() - startConnect < 60000))
  {
    arduino.write("startMonitor\n");
    //println("Connecting...");
    long now = millis();
    while (millis() - now < 1000) {
      ;
    }
    String input = getInput(false);
    if (input.length() > 4)
    {
      if (input.trim().equals("startBridge")) 
      {
        connected = true;
        println("true");
      }
    }
  }
  if (!connected) { 
    noLoop();
    println("No connection for Bridge...");
    text("NO BRIDGE CONNECTED TO " + portName, width/2, height/2);
  }
}
