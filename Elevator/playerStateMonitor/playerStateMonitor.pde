//27.12.2017, add Breathe progressbar filling
//03/01/2018 sliced receiving data of player states, new colors of UpGame
//12/06/2018 read data from input with regex...
import java.util.regex.Matcher;  
import java.util.regex.Pattern; 
import processing.serial.*;
Serial arduino;
boolean game;
boolean connected;
boolean startPeak1, startPeak2, bigPeak1, bigPeak2, lastPeak1, lastPeak2;
boolean startbigpause1, startbigpause2;
String portName;
PImage background;
float scrH, scrW;
float playerRectW, playerRectH;
float manX, manY, manW, manH;
float heartX, heartY, heartW, heartH;
float breatheX, breatheY, breatheW, breatheH;
float brBarX, brBarY, brBarW, brBarH, brBar1Fill, brBar2Fill;
float progBarX, progBarY, progBarH, progBarW, progBarOffset;
float pointX, pointY;
float newPointX, newPointY;
float dX, dY;
float devider = 60;
float progBarFill = 0.0; //(0..100 %)
float dotSize = 20;
float sine2Amp, sine1Amp, sineWidth, sineAlpha1, sineAlpha2;
float startPeakAmp, bigPeakAmp, lastPeakAmp;
float[] sineValues1, sineValues2;
int progBarPercent = 0;
int cnt = 0;

int count = 0;
PFont elevFont, prisonFont;
PImage PBG, PHeart, PBreathe, PMan;
long time1 = 0;
long time2 = 0;
long pause1time, pause2time;
int bFreq1 = 90;
int bFreq2 = 100;
int br1Data = 0;
int br1Min = 0;
int br1Max = 100;
int br1Offset = 1;
boolean pl1BrDanger = false;
int br2Data = 0;
int br2Min = 0;
int br2Max = 100;
int br2Offset = 1;
boolean pl2BrDanger = false;
int hr1State = 1;
int hr2State = 1;
int bd1State = 0;
int bd2State = 0;
//Background (30,35,36)
//stroke(0,195,206)
//Heart(222,0,0)
//White(254,255,255)
color redColor = color(255, 10, 10);
color blackColor = color(0, 0, 0);
color currColor = blackColor;
int level = 1;
int mainStart = 0;

void setup()
{
  //size(1200, 800);
  fullScreen();
  background(240);
  //Arduino connect
  arduinoConnect();

  background = loadImage("back.png");

  //Text settings
  textSize(20);
  prisonFont = createFont("Arial", 18, true);
  elevFont = createFont("Calibri", 16, true);
  textFont(elevFont, 15);

  //Calculate size and values and load images
  scrW = width;
  scrH = height;
  progBarOffset = 30;
  progBarW = 70;
  progBarH = scrH - 50;
  progBarX = scrW/2+scrH/2+progBarOffset;
  progBarY = scrH/2 - progBarH/2;
  playerRectW = 9*scrW/20;
  playerRectH = 9*scrH/20;
  pointX = scrW/2;
  pointY = scrH/2;
  PMan = loadImage("man.png");
  manX = scrW/4 - playerRectW/2 + 2*playerRectW/20;
  manY = scrH/2 - playerRectH/4;
  manH = PMan.height;
  manW = PMan.width;
  manW = (manW * (3*playerRectH/7)) / manH;
  manH = 3*playerRectH/7;
  PHeart = loadImage("heart.png");
  heartH = PHeart.height;
  heartW = PHeart.width;
  heartW = 4*(scrW/4 + playerRectW/20 - (manX + manW))/5;
  heartH = heartH * (heartW/PHeart.width);
  heartX = manX + manW + (scrW/4 + playerRectW/20 - (manX + manW) - heartW)/2;
  heartY = manY;
  PBreathe = loadImage("breathe.png");
  breatheX = heartX + heartW/10;
  breatheY = manY + manH - manH/5;
  breatheW = 8*heartW/10;
  breatheH = PBreathe.height * (breatheW/PBreathe.width);
  brBarX = scrW/4 + playerRectW/20;
  brBarY = scrH/2 + playerRectH/5;
  brBarW = 4*playerRectW/10;
  brBarH = playerRectH/20;
  brBar1Fill = brBarW/2;
  brBar2Fill = brBarW/2;

  sineWidth = 4*playerRectW/10;

  sineValues1 = new float[int(sineWidth)];
  sineValues2 = new float[int(sineWidth)];
  println(sineValues1.length);
  sineAlpha1 = 0.0;
  sineAlpha2 = 0.0;
  for (int c = 0; c < int(sineWidth); c++) 
  {
    sineValues1[c] = 0;
    sineValues2[c] = 0;
  }
  startPeak1 = false;
  bigPeak1 = false;
  lastPeak1 = false;
  startPeak2 = false;
  bigPeak2 = false;
  lastPeak2 = false;
  startbigpause1 = false;
  startbigpause2 = false;
  startPeakAmp = sineWidth/24;
  bigPeakAmp = sineWidth/3;
  lastPeakAmp = sineWidth/6;
  sine1Amp = startPeakAmp;
  sine2Amp = startPeakAmp;
}

void draw()
{
  //println("start try to read stream::"+str(millis()));
  String inData = getInput();
  //println("end try to read stream::"+str(millis()));
  //HR1=90 HR2=60 BD1=250 BD1Min=100 BD1Max=900 BD2=100 BD2Min=90 BD2Max=200
  if (inData.indexOf("vel=") > 0) { 
    if (inData.indexOf("level=") > -1) level = Integer.parseInt(inData.substring(6, inData.length()-1).trim());
    if (level == 2) 
    {
      level = 1;
      pl1BrDanger = true;
      pl2BrDanger = true;
    }
    if (level == 5) level = 4;
  }
  if (level == 1)
  {
    background(33, 34, 36);
    drawPlayerWindows();
    /*    
    String[] matches;
    if (inData.indexOf("HR1") >= 0 && inData.length() > 10)
    {
      String[] dataHR1 = split(inData,' ');
      if(dataHR1.length > 3)
      {
        if(dataHR1[0].indexOf("=") > 0)
        {
          matches = match(split(dataHR1[0],'=')[1], "\\d+");
          int digits = 0;
          if(matches.length > 0) digits = tryParse(matches[0]);
          if(digits > 0) hr1State = digits;
        }
        if(dataHR1[1].indexOf("=") > 0)
        {
          matches = match(split(dataHR1[1],'=')[1], "\\d+");
          int digits = 0;
          if(matches.length > 0) digits = tryParse(matches[0]);
          if(digits > 0) bFreq1 = digits;
        }
        if(dataHR1[2].indexOf("=") > 0)
        {
          matches = match(split(dataHR1[2],'=')[1], "\\d+");
          int digits = 0;
          if(matches.length > 0) digits = tryParse(matches[0]);
          if(digits > 0) hr2State = digits;
        }
        if(dataHR1[3].indexOf("=") > 0)
        {
          matches = match(split(dataHR1[3],'=')[1], "\\d+");
          int digits = 0;
          if(matches.length > 0) digits = tryParse(matches[0]);
          if(digits > 0) bFreq2 = digits;
        }
      }
    }
    
    if (inData.indexOf("BD1State") >= 0 && inData.length() > 10)
    {
      String[] dataBD1 = split(inData,' ');
      if(dataBD1.length > 4)
      {
        if(dataBD1[0].indexOf("=") > 0)
        {
          matches = match(split(dataBD1[0],'=')[1], "\\d+");
          int digits = 0;
          if(matches.length > 0) digits = tryParse(matches[0]);
          if(digits > 0) bd1State = digits;
        }
        if(dataBD1[0].indexOf("=") > 0)
        {
          matches = match(split(dataBD1[1],'=')[1], "\\d+");
          int digits = 0;
          if(matches.length > 0) digits = tryParse(matches[0]);
          if(digits > 0) br1Data = digits;
        }
        if(dataBD1[0].indexOf("=") > 0)
        {
          matches = match(split(dataBD1[2],'=')[1], "\\d+");
          int digits = 0;
          if(matches.length > 0) digits = tryParse(matches[0]);
          if(digits > 0) br1Min = digits;
        }
        if(dataBD1[0].indexOf("=") > 0)
        {
          matches = match(split(dataBD1[3],'=')[1], "\\d+");
          int digits = 0;
          if(matches.length > 0) digits = tryParse(matches[0]);
          if(digits > 0) br1Max = digits;
        }
        pl1BrDanger = split(dataBD1[4],'=')[1].trim().equals("RED");
      }
    }
    
    if (inData.indexOf("BD2State") >= 0 && inData.length() > 10)
    {
      String[] dataBD2 = split(inData,' ');
      if(dataBD2.length > 4)
      {
        if(dataBD2[0].indexOf("=") > 0)
        {
          matches = match(split(dataBD2[0],'=')[1], "\\d+");
          int digits = 0;
          if(matches.length > 0) digits = tryParse(matches[0]);
          if(digits > 0) bd2State = digits;
        }
        if(dataBD2[0].indexOf("=") > 0)
        {
          matches = match(split(dataBD2[1],'=')[1], "\\d+");
          int digits = 0;
          if(matches.length > 0) digits = tryParse(matches[0]);
          if(digits > 0) br2Data = digits;
        }
        if(dataBD2[0].indexOf("=") > 0)
        {
          matches = match(split(dataBD2[2],'=')[1], "\\d+");
          int digits = 0;
          if(matches.length > 0) digits = tryParse(matches[0]);
          if(digits > 0) br2Min = digits;
        }
        if(dataBD2[0].indexOf("=") > 0)
        {
          matches = match(split(dataBD2[3],'=')[1], "\\d+");
          int digits = 0;
          if(matches.length > 0) digits = tryParse(matches[0]);
          if(digits > 0) br2Max = digits;
        }
        pl2BrDanger = split(dataBD2[4],'=')[1].trim().equals("RED");
      }
    }
    */
    br1Data = br1Data + br1Offset;
    if (br1Data > br1Max) br1Offset = -1;
    if (br1Data < 1) br1Offset = 1;
    
    br2Data = br2Data + br2Offset;
    if (br2Data > br2Max) br2Offset = -1;
    if (br2Data < 1) br2Offset = 1;
    
    if (inData.indexOf("Main") >= 0 && inData.length() > 7)
    {
      int mainInd = inData.indexOf("MainStart=");
      if (mainInd > -1) mainStart = Integer.parseInt(inData.substring(mainInd + 10, inData.length()-1).trim());
    }
    strokeWeight(2);
    if(bFreq2 == 160) bFreq2 = int(random(100, 220));
    drawSine(bFreq1/6, bFreq2/6);
    strokeWeight(1);
    drawBreathe();
    text("Ready:" + str(mainStart>0), 750, height - 10);
    text("Prisoner 1: HeartState=" + str(hr1State>0) + " BreatheState=" + str(bd2State>0) + " HRate=" + str(bFreq1), 950, height - 70);
    text("Prisoner 2: HeartState=" + str(hr2State>0) + " BreatheState=" + str(bd1State>0) + " HRate=" + str(bFreq2), 150, height - 70);
  }
  if (level == 3) background(0);
  if (level == 4)
  {
    background(currColor);
    if (!inData.equals(" "))
    {
      float newX = 0;
      float newY = 0;
      int xIndex = inData.indexOf("X=");
      int yIndex = inData.indexOf("Y=");
      int pBarIndex = inData.indexOf("Bar = ");
      int percentIndex = inData.indexOf("%");
      int colorIndex = inData.indexOf("COL");

      if (inData.indexOf("COL=RED") > 0) currColor = redColor;
      else currColor = blackColor;
      if (xIndex > -1 && yIndex > -1 && pBarIndex > -1 && percentIndex > pBarIndex)
      {
        //println("start calculate X and Y::" + str(millis()));
        newX = tryParse(trim(inData.substring(xIndex+2, yIndex).trim()));
        newY = tryParse(trim(inData.substring(yIndex+2, colorIndex).trim()));
        progBarPercent = tryParse(trim(inData.substring(pBarIndex+5, percentIndex).trim()));
        if (newX > 500) newPointX = scrW/2 + ((newX-500)*(scrH/2-60))/500;
        else newPointX = scrW/2 - ((500-newX)*(scrH/2-60))/500;
        if (newY > 500) newPointY = scrH/2 - ((newY-500)*(scrH/2-60))/500;
        else newPointY = scrH/2 + ((500-newY)*(scrH/2-60))/500;
        dX = abs(newPointX - pointX);
        dY = abs(newPointY - pointY);
      }
      println("newPX=" + str(newPointX) + " newPY="+str(newPointY) + " Percent=" +str(progBarPercent));
      //println("end calculate X and Y::" + str(millis()));
    }
    stroke(0);
    fill(255);
    for (int j = 1; j < scrH/120-1; j++) ellipse(scrW/2, scrH/2, scrH - j*120, scrH - j*120);
    line(scrW/2-scrH/2, scrH/2, scrW/2+scrH/2, scrH/2); //Horiz Line
    line(scrW/2, scrH/2-scrH, scrW/2, scrH/2+scrH); //Verti Line
    rect(progBarX, progBarY, progBarW, progBarH);
    //Draw ProgressBar
    fill(0, 0, 255);
    progBarFill = progBarH * progBarPercent / 100;
    rect(progBarX, progBarY + (progBarH - progBarFill), progBarW, progBarFill);
    fill(255); // Text Color
    textSize(30);
    float ptW = textWidth(str(progBarPercent) + " %");
    if (progBarFill > 40) text(str(progBarPercent) + " %", progBarX + progBarW/2 - ptW/2, progBarY + progBarH - progBarFill + 35);
    else text(str(progBarPercent) + " %", progBarX + progBarW/2 - ptW/2, progBarY + progBarH - 5);
    fill(0);
    if (pointX < newPointX && abs(newPointX-pointX) > dX/devider) pointX = pointX + dX/devider;
    else if (pointX > newPointX && abs(pointX-newPointX) > dX/devider) pointX = pointX - dX/devider;
    if (pointY < newPointY && abs(newPointY-pointY) > dY/devider) pointY = pointY + dY/devider;
    else if (pointY > newPointY && abs(pointY-newPointY) > dY/devider) pointY = pointY - dY/devider;
    ellipse(pointX, pointY, dotSize, dotSize);
    stroke(255);
    fill(255);
    if(progBarPercent == 100 && abs(pointX - newPointX) < dX/devider && abs(pointY - newPointY) < dY/devider) level = 5;
    //text(str(frameRate),20,30);
  }
  if (level == 5)
  {
    //JPG
  }
}


void arduinoConnect()
{
  connected = false;
  portName = "COM3"; // COM3 or /dev/tty.wchusbserial1410 or /dev/tty.wchusbserial1420
  //portName = "/dev/tty.wchusbserial1411";
  //portName = "/dev/cu.usbmodem1421";
  ///arduino = new Serial(this, portName, 9600);
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
