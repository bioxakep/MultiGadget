/*
  People Controller 03/01/2018
  Connected to:
  A0, A1 pressSensors
  Next signal (digiread)
  Floor buttons (4x)
  added BreatheData sending to processing with min and max values
  add Breathe Min-Max and Current values calculating and sending to processing.
  02/jan/2018  up down buttons swaped
  03/01/2018 
  added sliced sending info of player states
  new algorithm of over-radius adaptation of dot
  08.01.2018 rewrite sending data to processing: each player separated sending
  08.01.2018 add ejected flag to players, add colors of breathe state.
  26/04/2018 hall sensors swapped

*/
#include <Wire.h>
#include "SoftwareSerial.h"

#define HALL1PIN  11 
#define HALL2PIN   8 
#define HALL3PIN  A0 
#define HALL4PIN  10 
#define HALL5PIN  12 
#define HALL6PIN   9 

#define HRLOW 60
#define HRMID 100
#define HRHIGH 160

#define BRE1PIN A7
#define BRE2PIN A6

#define MINTEMP 60

int i2cPCAddress = 20;
byte level = 1;
//============= BREATHE AND PULSE DATA ===============//
long breatheStopCount1 = 0;
long breatheStopCount2 = 0;
int breathePrevData1 = 0;
int breathePrevData2 = 0;
int delta1Array[3] = {0, 0, 0};
int delta2Array[3] = {0, 0, 0};
int breatheMinMax1[2] = {1024, 0};
int breatheMinMax2[2] = {1024, 0};
int breatheOffset = 8;   // 12 used for two week - release too soon
                          // better
boolean noBreatheMode1 = false;
boolean noBreatheMode2 = false;
boolean player1BreatheStoped = false;
boolean player2BreatheStoped = false;
boolean pla1Ejected = false;
boolean pla2Ejected = false;
int heartRate1 = HRLOW;
int heartRate2 = HRLOW;
byte HR1StopCount = 0;
byte HR2StopCount = 0;
boolean HR1State = true;
boolean HR2State = true;
boolean player1State = false;
boolean player2State = false;
//============ DARK SCREEN ===============//
boolean darkScreen = false;
boolean hallDone = false;
//============ HALL DATA ===============//
int hallSensorCount = 0;
//============ FLOOR DATA ===============//
boolean floorButt1State[2] = {true, true};
boolean floorButt2State[2] = {true, true};
boolean floorButt3State[2] = {true, true};
boolean floorButt4State[2] = {true, true};
boolean automotion = false;
long Radius = 500;
String currColor = "BLACK";
long curPoint[2] = {500, 500};
long tarPoint[2] = {500, 500};
long dx, dy;
int deviationScale = 1000; //Диаметр круга
unsigned long progressLift = 0;
unsigned long totalprogress = 0;
unsigned long maxVelocity = 50; //В цикл
unsigned long liftVelocity = maxVelocity;
int freqOfMath = 20;//сколько раз в секунду производим расчеты и сдвигаем точку с добавлением общего прохождения.
unsigned long waitDelay = 10000;
unsigned long floorDelay = 50;
unsigned long startWait = 0;
boolean upGameDone = false;
//============== FLOOR PINS ======================//
int floorButtonPins[4] = {5, 6, 4, 7}; // !!!!!!!!!!Left, Right, Down, Up!!!!!!!!!!!!
//============ COMMUNICATIONS =================//
//SoftwareSerial AFEXBEE(AFEXBEERX, AFEXBEETX); // RX, TX
boolean visOnly = true;
unsigned long sendHBTime = 0;
unsigned long tick = 0;
unsigned long sendXYTime = 0;

void setup()
{
  Serial.begin(9600);
  //AFEXBEE.begin(9600);
  Wire.begin(i2cPCAddress);
  Wire.onReceive(receiveMethod);
  Wire.onRequest(requestMethod);
  for (int k = 0; k < 4; k++) pinMode(floorButtonPins[k], INPUT_PULLUP);
  pinMode(HALL1PIN, INPUT_PULLUP);
  pinMode(HALL2PIN, INPUT_PULLUP);
  pinMode(HALL3PIN, INPUT_PULLUP);
  pinMode(HALL4PIN, INPUT_PULLUP);
  pinMode(HALL5PIN, INPUT_PULLUP);
  pinMode(HALL6PIN, INPUT_PULLUP);
  pinMode(BRE1PIN, INPUT);
  pinMode(BRE2PIN, INPUT);
  /*
while (true) {
  if (!digitalRead(HALL1PIN)) Serial.println("hall 111");
  if (!digitalRead(HALL2PIN)) Serial.println("hall    222");
  if (!digitalRead(HALL3PIN)) Serial.println("hall       333");
  if (!digitalRead(HALL4PIN)) Serial.println("hall          444");
  if (!digitalRead(HALL5PIN)) Serial.println("hall             555");
  if (!digitalRead(HALL6PIN)) Serial.println("hall                666");
delay(500);
}
 */
  
  floorDelay = 1000 / freqOfMath;
  totalprogress = 100 * maxVelocity * freqOfMath;
  progressLift = totalprogress * 0.2;
  noBreatheMode1 = false;
  noBreatheMode2 = false;
  randomSeed(A0);
  Serial.println("Wait for letsgame");
/*
while(automotion == false) {
  if (digitalRead(floorButtonPins[0]) == LOW) Serial.println (" button 0000");
  if (digitalRead(floorButtonPins[1]) == LOW) Serial.println (" button     1111");
  if (digitalRead(floorButtonPins[2]) == LOW) Serial.println (" button         2222");
  if (digitalRead(floorButtonPins[3]) == LOW) Serial.println (" button             3333");
  delay(300);
}
*/  
  connectToMonitor();
  breathePrevData1 = analogRead(BRE1PIN);
  breathePrevData2 = analogRead(BRE2PIN);
  breatheMinMax1[0] = breathePrevData1;
  breatheMinMax1[1] = breathePrevData1;
  breatheMinMax2[0] = breathePrevData2;
  breatheMinMax2[1] = breathePrevData2;
  sendHBTime = millis();
  sendXYTime = millis();

  
}
void loop()
{
  tick = millis();
  if (level == 1) // HR and Breathe STAGE
  {
    if (tick - sendHBTime >= 500)
    {
      breatheMeasure();
      //sendHrData(heartRate1, heartRate2);
      sendHBTime = tick;
    }
    //pulseMeasure();
    /*if (!visOnly)
    {
      if (!HR1State && player1BreatheStoped) player1State = true;
      if (!HR2State && player2BreatheStoped) player2State = true;
    }
    */
  }
  else if (level == 2) // HOLL STAGE
  {
    delay(500);
    Serial.println("level 2, waiting for magnet");
    /*if (!digitalRead(HALL3PIN) || !digitalRead(HALL4PIN))
    {
      if(hallSensorCount == 0) sendHrData(heartRate1, HRLOW);  // HRHIGH
      hallSensorCount++;
      Serial.println("hallSensorCount increased and = " + String(hallSensorCount));
    }
    else 
    {
      sendHrData(0, 0);
      hallSensorCount = 0;
      Serial.println("hallSensorCount = 0");
    }
    
    if(hallSensorCount >= 6) 
    {
      hallDone = true;
      Serial.println("Hall Sensor Done!");
    }*/
  }
  else if (level == 4) // FLOOR STAGE
  {
    if (tick - startWait >= waitDelay)
    {
      boolean radiusSearch = true;
      long x, y;
      long r;
      while (radiusSearch)
      {
        x = (long)random(0, deviationScale);
        y = (long)random(0, deviationScale);
        r = hypoLength(x, y);
        if (r <= Radius && r >= 4 * Radius / 5) radiusSearch = false; //Если точка попала в кольцо от 4/5 радиуса до радиуса то играем.
      }
      tarPoint[0] = x;
      tarPoint[1] = y;
      //Serial.println("X="+String(tarPoint[0]) + " Y="+String(tarPoint[1]) + " R="+String(r));
      dx = tarPoint[0] - curPoint[0];
      dy = tarPoint[1] - curPoint[1];
      //Serial.println("DX="+String(dx)+" DY="+String(dy));
      startWait = tick;
      automotion = true;
    }
    delay(floorDelay);

    //Уводим лифт в сторону новой точки
    if (automotion)
    {
      if (dx < 0) curPoint[0] -= abs(dx) / (5 * freqOfMath);
      else curPoint[0] += dx / (5 * freqOfMath);
      if (dy < 0) curPoint[1] -= abs(dy) / (5 * freqOfMath);
      else curPoint[1] += dy / (5 * freqOfMath);
      //Serial.println("cP-tP (X) = "+String(curPoint[0] - tarPoint[0]));
      //Serial.println("cP-tP (Y) = "+String(curPoint[1] - tarPoint[1]));
      
      if (abs(curPoint[0] - tarPoint[0]) < abs(dx) / (5 * freqOfMath)) {
        curPoint[0] = tarPoint[0];
        automotion = false;
      }
      if (abs(curPoint[1] - tarPoint[1]) < abs(dy) / (5 * freqOfMath)) {
        curPoint[1] = tarPoint[1];
        automotion = false;
      }
    }

    //Нажатием кнопок корректируем направление
    for (int b = 0; b < 4; b++) // Left, Right, Down, Up
    {
      boolean tapped = !digitalRead(floorButtonPins[b]);
      if (tapped)
      {
        automotion = false;
        if (b/2 == 0) // X
        {
          if (b % 2 == 0 && hypoLength(curPoint[0] - 3, curPoint[1]) <= Radius) { curPoint[b / 2] -= 3; tarPoint[b / 2] = curPoint[b / 2]; }
          if (b % 2 == 1 && hypoLength(curPoint[0] + 3, curPoint[1]) <= Radius) { curPoint[b / 2] += 3; tarPoint[b / 2] = curPoint[b / 2]; }
        }
        if (b/2 == 1) // Y
        {
           if (b % 2 == 0 && hypoLength(curPoint[0], curPoint[1] - 3) <= Radius) { curPoint[b / 2] -= 3; tarPoint[b / 2] = curPoint[b / 2]; }
           if (b % 2 == 1 && hypoLength(curPoint[0], curPoint[1] + 3) <= Radius) { curPoint[b / 2] += 3; tarPoint[b / 2] = curPoint[b / 2]; }
        }
      }
    }

    //Расчет данных для процессинга и отправка.
    long currRadius = hypoLength(curPoint[0], curPoint[1]);
    if(currRadius > 333) currColor = "RED";
    else currColor = "BLACK";
    //Serial.println("cR="+String(currRadius)+" cR/R="+String((float)currRadius / Radius));
    progressLift += (maxVelocity - (maxVelocity * currRadius / Radius));
    
    if(tick - sendXYTime > 700)
    {
      long percent = 100 * progressLift / totalprogress;
      Serial.println("NormProgressBar = " + String(percent) + " % X=" + String(curPoint[0]) + " Y=" + String(curPoint[1]) + " COL="+currColor);
      sendXYTime = tick;
    }

    if (progressLift >= totalprogress)
    {
      upGameDone = true; // Конец игры
      level++;
      delay(500);
      Serial.println("ProgressBar = 100 % X=500 Y=500 COL=BLACK");
    }
  }
  else if (level == 5)
  {
    //Win processing command
    Serial.println("ProgressBar = 100 % X=500 Y=500 COL=BLACK");
    delay(1000);
  }
}

long hypoLength(long _x, long _y)
{
  long x2 = (_x - Radius) * (_x - Radius);
  long y2 = (_y - Radius) * (_y - Radius);
  long ret = sqrt(x2+y2);
  //Serial.println("X2="+String(x2) + " Y2="+String(y2) + " SQ=" + String(ret));
  return ret;
}

void receiveMethod(int howMany)
{
  if (Wire.available() > 0)
  {
    byte cmd = Wire.read();
    if (cmd == 0xFF) 
    {
      visOnly = false;
      Serial.println("MainStart=" + String(!visOnly));
    }
    else
    {
      level = (int)cmd;
      Serial.println("level=" + String(level));
    }
  }
}

void requestMethod()
{
  if (level == 1)
  {
    byte playerStates = 0x00;
    if (player1State) playerStates |= 1 << 2;
    if (player2State) playerStates |= 1 << 6;
    Wire.write(playerStates);
  }
  else if (level == 2) Wire.write(hallDone | 0x00);
  else if (level >= 4) Wire.write(upGameDone | 0x00);
}

boolean readButt(boolean prevState, int pin)
{
  boolean currState = digitalRead(pin);
  if (currState != prevState)
  {
    delay(5);
    currState = digitalRead(pin);
  }
  return currState;
}

void sendHrData(int _hr1, int _hr2)
{
  Serial.println("HR1State=" + String(HR1State) + " HR1=" + String(_hr1) + " HR2State=" + String(HR2State) + " HR2=" + String(_hr2));
}
