import ddf.minim.*;
import processing.serial.*;

PFont font;
PImage background;
String portName;
Serial myPort;

int level;

boolean firstStart = true;
boolean game = false;
boolean controllerConnected = false;

float scrW, scrH, elevOff, elevGridSizeH, logoBoxW, logoBoxH, elevBoxW, elevBoxH;
float logoW, logoH;
//float elevBoxGridUp = дописать  

void setup()
{
  String[] comportConfig = loadStrings("com.txt");
  portName = comportConfig[0].substring(comportConfig[0].indexOf("\"") + 1, comportConfig[0].length() - 2);
  //if(portName.length() > 0) myPort = new Serial(this, portName, 9600);
  level = 0;
  fill(255);
  stroke(50);
  font = createFont("Arial Bold", 14);
  textFont(font);
  //background = loadImage("background_main.jpg");
  size(600, 500);
  //fullScreen();
  background(30, 35, 36);  
  scrW = width;
  scrH = height;
  elevBoxW = scrW * 0.45;
  elevBoxH = scrH * 0.7;
  elevGridSizeH = elevBoxH * 0.95;
  logoBoxH = elevBoxH; 
  logoBoxW = scrW - elevBoxW;
  
  logoW = 0.8 * logoBoxW;
  logoH = 0.8 * logoBoxH;
  //TEST
  game = true;
  level = 0;
  //TEST
}

void draw()
{
  if (game)
  {
    background(0);
    //getInput();
    if (level == 0) //C1 State
    {
      background(30, 35, 36);
      fill(200);
      stroke(0);
      //Main separators
        line(elevBoxW,0,elevBoxW,scrH);
        line(0,elevBoxH,scrW,elevBoxH);
      //Elements
        drawElevBox();
        drawLogo();
        drawStats();
    } else if (level == 1)
    {

    } else if (level == 2)
    {
      
    } else if (level == 3)
    {

    } else if(level == 4)
    {
      
    }
  } else
  {
    if (portName.length() == 0) 
    {
      textAlign(CENTER);
      fill(255);
      textSize(50);
      text("CONNECT ARDUINO TO PC AND RESTART", width/2, height/2 - 20);
      text("(PRESS ESC TO EXIT)", width/2, height/2 + 70);
    } else
    {
      if(firstStart) 
      {
        myPort.stop();
        myPort = new Serial(this, portName, 9600);
        //myPort = new Serial(this, Serial.list()[1], 9600);
        firstStart = false;
      }
      background(0);
      myPort.write("MONCACTIVE\n"); 
      long now = millis();
      println("try Connect");
      while (millis() - now < 1000) {;}
      getInput();
    }
  }
}

void getInput()
{
  if (myPort.available() > 0)
  {
    String inp = myPort.readStringUntil('\n');
    if (inp != null)
    {
      if (inp.length() > 4) 
      {
        println(inp);
        if(inp.substring(0, 4).equals("LVL:"))
        {
          int newlevel = int(inp.substring(4,5));
          if (newlevel > 0) level = newlevel; 
        }
        if (inp.substring(0, 9).equals("MONCSTART")) 
        { 
          game = true;
          controllerConnected = true;
          level = 0;
        }
      }
    } 
  }
}

void drawElevBox()
{
  
}
void drawLogo()
{
  rect(elevBoxW + (logoBoxW - logoW)/2, (logoBoxH - logoH)/2, logoW, logoH);
}
void drawStats()
{
}