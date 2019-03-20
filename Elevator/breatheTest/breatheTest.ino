#include "SoftwareSerial.h"
#define HRLOW 60
#define HRMID 90
#define HRHIGH 150

#define AFEXBEERX 3
#define AFEXBEETX 2

#define BRE1PIN A7
#define BRE2PIN A6

long breatheStopCount1 = 0;
long breatheStopCount2 = 0;
int breathePrevData1 = 0;
int breathePrevData2 = 0;
int delta1Array[3] = {0, 0, 0};
int delta2Array[3] = {0, 0, 0};
int breatheMinMax1[2] = {1024, 0};
int breatheMinMax2[2] = {1024, 0};
int breatheOffset = 17;
boolean noBreatheMode1 = false;
boolean noBreatheMode2 = false;
boolean player1BreatheStoped = false;
boolean player2BreatheStoped = false;
unsigned long BreatheDelay = 500;

//SoftwareSerial AFEXBEE(AFEXBEERX, AFEXBEETX); // RX, TX

void setup() {
  Serial.begin(9600);
  //AFEXBEE.begin(9600);
  pinMode(BRE1PIN, INPUT);
  pinMode(BRE2PIN, INPUT);
  noBreatheMode1 = false;
  noBreatheMode2 = false;
  randomSeed(A0);
  Serial.println("Wait for letsgame");
  connectToMonitor();
  breathePrevData1 = analogRead(BRE1PIN);
  breathePrevData2 = analogRead(BRE2PIN);
  breatheMinMax1[0] = breathePrevData1;
  breatheMinMax1[1] = breathePrevData1;
  breatheMinMax2[0] = breathePrevData2;
  breatheMinMax2[1] = breathePrevData2;
}

void loop()
{
  delay(BreatheDelay);
  int breatheData1 = analogRead(BRE1PIN);
  int breatheData2 = analogRead(BRE2PIN);
  if (breatheMinMax1[0] > breatheData1) breatheMinMax1[0] = breatheData1;
  if (breatheMinMax1[1] < breatheData1) breatheMinMax1[1] = breatheData1;
  if (breatheMinMax2[0] > breatheData2) breatheMinMax2[0] = breatheData2;
  if (breatheMinMax2[1] < breatheData2) breatheMinMax2[1] = breatheData2;
  int delta1 = abs(breatheData1 - breathePrevData1);
  int delta2 = abs(breatheData2 - breathePrevData2);
  Serial.println("BD1=" + String(breatheData1) + " BD1Min=" + String(breatheMinMax1[0]) + " BD1Max=" + String(breatheMinMax1[1]) + " Delta1 = " + String(delta1));
  Serial.println("BD2=" + String(breatheData2) + " BD2Min=" + String(breatheMinMax2[0]) + " BD2Max=" + String(breatheMinMax2[1]) + " Delta2 = " + String(delta2));
  int maxDelta1 = 0;
  int maxDelta2 = 0;
                          // ============ PLAYER 1 =====================
  if (!player1BreatheStoped)
  {
    if (delta1 <= breatheOffset)
    {
      if (!noBreatheMode1)
      {
        breathePrevData1 = breatheData1;
        noBreatheMode1 = true;
        Serial.println("STOP BREATHE 1 PLAYER");
      }
      else
      {
        delta1Array[breatheStopCount1 % 3] = delta1;
        Serial.print("Delta1 Array::");
        for (int g = 0; g < 3; g++)
        {
          if (maxDelta1 < delta1Array[g]) maxDelta1 = delta1Array[g];
          Serial.print(String(delta1Array[g]) + "|");
        }
        breatheStopCount1++;
        Serial.println("| BRCNT1=" + String(breatheStopCount1));
      }
    }
    else
    {
      Serial.println("START BREATHE 1 PLAYER");
      noBreatheMode1 = false;
      breatheStopCount1 = 0;
      breathePrevData1 = breatheData1;
      for (int g = 0; g < 3; g++) delta1Array[g] = 0;
    }
  }

  if (breatheStopCount1 > 20)
  {
    Serial.println("Stop breathe of player 1 detected");
    player1BreatheStoped = true;
    breatheStopCount1 = 0;
  }

  if (breatheStopCount1 > 2 && maxDelta1 < 3 && breatheData1 > 180)
  {
    Serial.println("Eject player 1 from wall detected");
    player1BreatheStoped = true;
    breatheStopCount1 = 0;
  }
                      // ============ PLAYER 2 =====================
  if (!player2BreatheStoped)
  {
    if (delta2 <= breatheOffset)
    {
      if (!noBreatheMode2)
      {
        breathePrevData2 = breatheData2;
        noBreatheMode2 = true;
        Serial.println("STOP BREATHE 2 PLAYER");
      }
      else
      {
        delta2Array[breatheStopCount2 % 3] = delta2;
        Serial.print("Delta2 Array::");
        for (int g = 0; g < 3; g++)
        {
          if (maxDelta2 < delta2Array[g]) maxDelta2 = delta2Array[g];
          Serial.print(String(delta2Array[g]) + "|");
        }
        breatheStopCount2++;
        Serial.println("| BRCNT2=" + String(breatheStopCount2));
      }
    }
    else
    {
      Serial.println("START BREATHE 2 PLAYER");
      noBreatheMode2 = false;
      breatheStopCount2 = 0;
      breathePrevData2 = breatheData2;
      for (int g = 0; g < 3; g++) delta2Array[g] = 0;
    }
  }

  if (breatheStopCount2 > 20)
  {
    Serial.println("Stop breathe of player 2 detected");
    player2BreatheStoped = true;
    breatheStopCount2 = 0;
  }

  if (breatheStopCount2 > 2 && maxDelta2 < 3 && breatheData2 > 200)
  {
    Serial.println("Eject player 2 from wall detected");
    player2BreatheStoped = true;
    breatheStopCount2 = 0;
  }
}

void connectToMonitor()
{
  boolean monConnected = false;
  while (!monConnected)
  {
    if (Serial.available() > 0)
    {
      String input = "";
      input = Serial.readStringUntil('\n');
      if (input.startsWith("letsgame"))
      {
        Serial.println("start ver 008 both players");
        delay(500);
        digitalWrite(13, HIGH);
        boolean sync = false;
        while (!sync)
        {
          if (Serial.available() > 0)
          {
            Serial.readStringUntil('\n');
            delay(50);
            Serial.println("start");
            delay(500);
          }
          else sync = true;
        }
        monConnected = true;
      }
    }
  }
}
