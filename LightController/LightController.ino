// Light Controller v1 (beta)
// 17 AGO 2018
// I2C enabled - slave for master // xBee removed - receives commands from Master and World
// 23 AGO 2018 flowers added
// 13 Sep 2018 windows and colors corrected

// bright on all windows
// red on selected window
// blue on selected window
// green on all windows
// dark on inactive windows

/// PWM color control for windows
#include "Wire.h" // I2C

int oknoA_R   = 6;
int oknoA_G   = 5;
int oknoA_B   = 4;

int oknoB_R   =  9;
int oknoB_G   =  8;
int oknoB_B   =  7;

int oknoC_R   =  12;
int oknoC_G   =  11;
int oknoC_B   =  10;

int dvor_W    = 46;
int dvor_R    = 45;
int dvor_G    = 3;
int dvor_B    = 2;

int wind      = A8; //  AC relay to control Wind blower
int mainLight = A2; //  main light in game, will turn on after ballon
int underLight = A7; //  (U light)relay to control underground lighting (not used in game but remotely by operator)
int extraLight = 41;

int crystPins[3] = {23,24,25};

int flowerR   = 53;
int flowerB   = 51;
int fastled   = 7;

int motor     = 49;
int north     = 47;
int turner    = 43;

byte command = 0;
int thisI2CAddr = 20;

int minWindPause = 100;
int maxWindPause = 900;
int minWindTime = 1500;
int maxWindTime = 3000;

unsigned long windTime = 0;
unsigned long windPause = 0;
unsigned long startWind = 0;
unsigned long stopWind = 0;
unsigned long blowTime = 10000;
boolean windState = false;
boolean randomWind = false;

int crystRecPin = 48;
boolean crystStates[3] = {false, false, false};
boolean flowerBState = false;
int turnCount = 0;
byte dir = 0;
byte prevPosInds[4] = {1, 0, 2, 0};
byte currPosInds[4] = {1, 0, 2, 0};
int colorPins[4][2] = {{10, 12}, {7, 9}, {4, 6}, {2, 45}};
String places[4] = {"OknoA", "OknoB", "OknoC", "Dvor"};
String colors[3] = {"BLK", "BLU", "RED"};
int lStep = 10;

boolean moonSun = true; // TEST


void setup()
{
  Serial.begin(9600);
  Wire.begin(thisI2CAddr);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  pinMode(oknoA_R, OUTPUT);
  pinMode(oknoA_G, OUTPUT);
  pinMode(oknoA_B, OUTPUT);

  pinMode(oknoB_R, OUTPUT);
  pinMode(oknoB_G, OUTPUT);
  pinMode(oknoB_B, OUTPUT);

  pinMode(oknoC_R, OUTPUT);
  pinMode(oknoC_G, OUTPUT);
  pinMode(oknoC_B, OUTPUT);

  pinMode(dvor_W, OUTPUT);
  pinMode(dvor_R, OUTPUT);
  pinMode(dvor_G, OUTPUT);
  pinMode(dvor_B, OUTPUT);

  pinMode(wind,   OUTPUT);
  pinMode(underLight, OUTPUT);
  pinMode(extraLight, OUTPUT);
  pinMode(turner, INPUT_PULLUP);
  pinMode(north, INPUT_PULLUP);

  pinMode(flowerR, OUTPUT);
  pinMode(flowerB, OUTPUT);

  digitalWrite(flowerR, LOW);
  digitalWrite(flowerB, LOW);
  digitalWrite(wind,    LOW);
  digitalWrite(underLight, HIGH);
  digitalWrite(extraLight, LOW);

  for(int c = 0; c < 3; c++)
  {
    pinMode(crystPins[c], INPUT_PULLUP);
  }

  Serial.println("\nLight Controller v1  \nSep_2018 ");
  Serial.println("Hardware = Mega\n");

  //testing();

  for(int c = 0; c < 3; c++)
  {
    Serial.println("Crystal "+String(c)+"  = " + String(digitalRead(crystPins[c])));
  }
  
  Serial.println("turner     = " + String(digitalRead(turner)));
  randomSeed(A0);

  setLightBri(0);
  Serial.println("\nReady\n");

}

void loop() {

  if (moonSun)
  {
    // считаем число замыканий, делим на 3. если счетчик увеличился
    // сменяем цвета в цикле и мотор на 12 сек
    // если срабатывает north сравниваем 0 у нас или нет в модуле и если нет - обновляем счетчик
    if (!digitalRead(turner))
    {
      turnCount++;
      //Serial.print("Turn detected");
      delay(100);
    }
    if (!digitalRead(north))
    {
      dir = 0;
      turnCount = 0;
    }
    if (turnCount == 3) // BLK -- RED -- BLK -- BLU
    {
      //Windows Color Control
      Serial.print("Next Turn::");
      turnCount = 0;
      dir = (dir + 1) % 4;
      for (int p = 0; p < 4; p++)
      {
        byte currCol = currPosInds[(p + dir) % 4]; // dir - текущее направление (N,E,W,S), currCol - цвет для конкретного окна в текщем направлении.
        byte prevCol = currPosInds[(p + dir - 1) % 4]; // Для будущего написания плавности, цвет для конкретного окна в предыдущем направлении.

        Serial.print(String(places[p]) + " is " + String(colors[currCol]));
        if (prevCol > currCol)
        {
          for (int r = 255; r > 0; r = r - lStep) analogWrite(colorPins[p][prevCol], r);
        }
        else
        {
          for (int r = 0; r > 255; r = r + lStep) analogWrite(colorPins[p][currCol], r);
        }
      }
      //Flowers Control
      digitalWrite(flowerB, dir == 0);
      digitalWrite(flowerR, dir == 2);
      Serial.print(", FloB is " + String(dir == 0));
      Serial.println(", FloR is " + String(dir == 2));
    }
  }

  if (command > 0) Serial.println("Command = " + String(command));
  
  if (command == 0x10) setLightBri(0); // Выключить весь свет
  else if (command == 0x11) randomWind = true;
  else if (command == 0x12) // Baloon passed
  {
    randomWind = false;
    windState = false;
    digitalWrite(wind, windState);
    setLightBri(125); // okna 50 % all
  }
  else if(command == 0x31)
  {
    // wind for 10-15 secs
    startWind = millis();
    windState = true;
    digitalWrite(wind, windState);
  }
  else if(command == 0x41)
  {
    // wind for 10-15 secs
    moonSun = true;
  }
  else if(command == 0x42)
  {
    // wind for 10-15 secs
    moonSun = false;
    setLightBri(0); // okna 50 % all
  }
  else if(command == 0x55)
  {
    digitalWrite(crystRecPin, LOW);
  }
  command = 0;
  
  if (randomWind) randWind();
  else if (windState) // for stop wind after comand 0x80
  {
    if (millis() - startWind > blowTime)
    {
      windState = false;
      digitalWrite(wind, windState);
    }
  }

  for(int c = 0; c < 3; c++)
  {
    if(!digitalRead(crystPins[c]) && !crystStates[c])
    {
      delay(10);
      if(!digitalRead(crystPins[c]))
      {
        crystStates[c] = true;
      }
    }
  }

  /*

    North = 1
    West  = 2
    South = 3                              1 2 3 4
    East  = 4                              N W S E
    //if command == North >   oknoA = blue 1 0 2 0
    //                        oknoB = dark 0 1 0 2
    //                        oknoC = red  2 0 1 0
    //                        dvor  = dark 0 2 0 1
    //                        flowerB = OFF

    //if command == West  >   oknoA = dark 0
    //                        oknoB = blue 1
    //                        oknoC = dark 0
    //                        dvor  = red  2
    //                        flowerR = ON

    //if command == South >   oknoA = red  2
    //                        oknoB = dark 0
    //                        oknoC = blue 1
    //                        dvor  = dark 0
    //                        flowerR = OFF

    //if command == East  >   oknoA = dark 0
    //                        oknoB = red  2
    //                        oknoC = dark 0
    //                        dvor  = blue 1
    //                        flowerB = ON
  */
  // wait for command from master via i2c (most commands to control oknoX cames from World (thru Master I think)

  //if command == windRandom > turn on wind randomly until command windStop is recieved from master
  //if command == windStop   > stop the wind
  //if command == windBlow   > execute sequence wind_is_blowing for 10 seconds
  //
  //if command == light_dark > turn all light off
  //if command == light_bright > turn all light on
  //
  //if command == light_green  > turn all green lights on, everything else is off
  //
  //if command == under > toggle underlight on/off
  //
  //if command == north >   oknoA = blue
  //                        oknoB = dark
  //                        oknoC = red
  //                        dvor  = dark
  //
  //if command == west  >   oknoA = dark
  //                        oknoB = blue
  //                        oknoC = dark
  //                        dvor  = red
  //                        flowerR = ON

  //if command == south >   oknoA = red
  //                        oknoB = dark
  //                        oknoC = blue
  //                        dvor  = dark
  //
  //if command == east  >   oknoA = dark
  //                        oknoB = red
  //                        oknoC = dark
  //                        dvor  = blue
  //                        flowerB = ON

  // final part of the game ---CRYSTALS---
  //if (!digitalRead(cryst1) && !digitalRead(cryst2) && !digitalRead(cryst3)) send command crystDone to master (GAME OVER)

}

void receiveEvent(int howMany)
{
  if (Wire.available() > 0) // пройтись по всем до последнего
  {
    command = Wire.read();    // принять байт как символ
  }
  /*
     commands
     0x10 =
  */
}

void requestEvent()
{
  byte ans = 0x00;
  for (int i = 0; i < 3; i++)
  {
    if (crystStates[i]) ans |= (1 << i*2);
  }
  Wire.write(ans);
}

