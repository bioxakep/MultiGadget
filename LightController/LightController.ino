// Light Controller v1
// 17 AGO 2018
// I2C enabled - slave for master // xBee removed - receives commands from Master and World
// 23 AGO 2018 flowers added
// 13 Sep 2018 windows and colors corrected
// 18 NOV 2018 flower start adjusted
// 27 NOV 2018 underlight deleted/moved to motor controller
// 28 NOV 2018 light control while Ghera speaks added
// 01 DEC 2018 wire speed to 10000
// 13 DEC 2018 crystals set to new pins, tested, HIGH then crystal present
//
/// PWM color control for windows
#include "Wire.h" // I2C

int oknoA_R   =  6;
int oknoA_G   =  5;
int oknoA_B   =  4;

int oknoB_R   =  9;
int oknoB_G   =  8;
int oknoB_B   =  7;

int oknoC_R   =  12;
int oknoC_G   =  11;
int oknoC_B   =  10;

int dvor_W    =  46;
int dvor_R    =  45;
int dvor_G    =  3;
int dvor_B    =  2;

int wind      = A8; //  AC relay to control Wind blower
int mainLight = A2; //  main light in game, will turn on after ballon
//int underLight = A7; //  (U light)relay to control underground lighting (not used in game but remotely by operator)
int extraLight = A7;

int crystPins[3] = {41, 39, 37}; // {23,25,27};

int flowerR   = 53;
int flowerB   = 51;
// int fastled   = 7;  // not used

int motor     = 49;  // now is just a pulse signal to change a picture on a video player
//int freeInput     = 47;  // not used anymore
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

//int crystRecPin = 48;
boolean crystStates[3] = {false, false, false};
boolean flowerBState = false;
int turnCount = 0;
byte dir = 0;
byte prevPosInds[4] = {2, 0, 1, 0};
byte currPosInds[4] = {2, 0, 1, 0};
int colorPins[4][2] = {{10, 12}, {7, 9}, {4, 6}, {2, 45}};
String places[4] = {"OknoA", "OknoB", "OknoC", "Dvor"};
String colors[3] = {"BLK", "BLU", "RED"};
int lStep = 10;
long turnMoment = 0;
bool moonSun = true; // TEST

long lightTimer = 0;
long lightDelay = 0;

void setup() {
  Wire.begin(thisI2CAddr);
  Wire.setClock(10000);
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

  pinMode(motor,      OUTPUT);
  pinMode(wind,       OUTPUT);
  pinMode(extraLight, OUTPUT);
  pinMode(mainLight,  OUTPUT);

  pinMode(turner,  INPUT_PULLUP);
  //  pinMode(freeInput,  INPUT_PULLUP);

  digitalWrite(extraLight, HIGH);  // LOW = ON
  digitalWrite(mainLight,  HIGH);  // LOW = ON
  pinMode(flowerR, OUTPUT);
  pinMode(flowerB, OUTPUT);

  digitalWrite(flowerR,    LOW);
  digitalWrite(flowerB,    LOW);
  digitalWrite(wind,        LOW);
  digitalWrite(motor,       LOW);

  Serial.begin(9600);

  for (int c = 0; c < 3; c++)
  {
    pinMode(crystPins[c], INPUT_PULLUP);
    Serial.println("Crystal " + String(c) + "  = " + String(digitalRead(crystPins[c])));
  }

  Serial.println("\nLight Controller v1  \nSep_2018 - 01.dec.2018 - 13.DEC.2018");
  Serial.println("Hardware = Mega\n");

  testing();

  Serial.println("\nturner     = " + String(digitalRead(turner)));

  //  while (true) {
  //    testA();
  //  }


  randomSeed(A0);

  Serial.println("\nReady\n");
  setLightBri(250);
  //  analogWrite(dvor_W, 255-0);
  //  digitalWrite(dvor_R, HIGH);
  //  digitalWrite(dvor_G, HIGH);
  //  digitalWrite(dvor_B, HIGH);
  delay(1000);
  digitalWrite(flowerR,    HIGH);
  digitalWrite(flowerB,    HIGH);
  digitalWrite(extraLight, LOW);  // LOW = ON
  digitalWrite(mainLight,  LOW);  // LOW = ON

}

void loop() {

  if (moonSun)
  {
    // считаем число замыканий, делим на 3. если счетчик увеличился
    // сменяем цвета в цикле и ///  мотор на 12 сек
    // если срабатывает ///bigKey сравниваем 0// у нас или нет в модуле и если нет - обновляем счетчик
    // bigKey больше не существует тут
    // посылаем короткий сигнал на мотор каждый раз когда меняем положение sun - moon

    if (!digitalRead(turner) && millis() - turnMoment > 3000)
    {
      turnCount++;
      //Serial.print("Turn detected");
      delay(100);
    }

    if (turnCount == 3) // BLK -- RED -- BLK -- BLU
    {
      shiftPic();
      delay(2000);
      turnMoment = millis();
      //Windows Color Control
      Serial.print("Next Turn::");
      turnCount = 0;
      dir = (dir + 1) % 4;
      for (int p = 0; p < 4; p++)
      {
        int currCol = currPosInds[(p + dir) % 4] - 1; // dir - текущее направление (N,E,W,S), currCol - цвет для конкретного окна в текщем направлении.
        int prevCol = currPosInds[(p + dir - 1) % 4] - 1; // Для будущего написания плавности, цвет для конкретного окна в предыдущем направлении.

        Serial.print(String(places[p]) + " is " + String(colors[currCol + 1]));
        // currCol in (-1,0,1)
        // prevCol in (-1,0,1)
        if (prevCol > currCol) // prev = 0|1, curr = -1, not else
        {
          if (p < 3) for (int r = 255; r > 0; r = r - lStep) analogWrite(colorPins[p][prevCol], max(r, 0)); //light-down prevColorPin in window "p"
          else for (int r = 0; r < 255; r = r + lStep) analogWrite(colorPins[p][prevCol], min(r, 255));
        }
        else // curr = 0|1, prev = -1, not else
        {
          if (p < 3) for (int r = 0; r < 255; r = r + lStep) analogWrite(colorPins[p][currCol], min(r, 255)); //light-Up currColorPin in window "p"
          else for (int r = 255; r > 0; r = r - lStep) analogWrite(colorPins[p][currCol], max(r, 0));
        }
      }
      //Flowers Control
      digitalWrite(flowerB, dir == 1);
      digitalWrite(flowerR, dir == 3);
      Serial.print(", FlowerB is " + String(dir == 0));
      Serial.println(", FloweR is " + String(dir == 2));
    }
  } // eof_moonSun
  else
  {
    byte cristals = 0;
    for (int c = 0; c < 3; c++)
    {
      if (!digitalRead(crystPins[c]) && !crystStates[c])
      {
        delay(10);
        if (!digitalRead(crystPins[c])) {
          crystStates[c] = true;
          cristals++;
        } else {
          crystStates[c] = false;
        }
        //  with every crytal in place lets light up oknos, red for example, if 1  - 30% , if 2  - 60% and then all 100% red
      }
    }
    analogWrite(oknoA_R, cristals * 75);
    analogWrite(oknoB_R, cristals * 75);
    analogWrite(oknoC_R, cristals * 75);
  }


  //-------------------------------------------------------------------------------------------

  if (command > 0) Serial.println("Command = " + String(command, HEX));

  if (command == 0x10) // PreStart
  {
    digitalWrite(wind,    HIGH);
    digitalWrite(flowerR, LOW);
    delay(500);
    digitalWrite(flowerB, LOW);
    digitalWrite(wind,    LOW);
    command = 0;

  }
  else if (command == 0x11) // main start
  {
    setLightBri(0); // Выключить весь свет
    randomWind = true;
    digitalWrite(mainLight, HIGH); // HIGH = turn off all the light
    digitalWrite(extraLight, HIGH); // OFF
    shiftPic();  // move son-moon to black screen
    command = 0;
  }
  else if (command == 0x12) // Baloon passed
  {
    randomWind = false;
    windState = false;
    digitalWrite(wind, windState);
    digitalWrite(mainLight,  LOW);  //ON
    digitalWrite(extraLight, HIGH); //OFF

    analogWrite(dvor_W, 125);
    analogWrite(dvor_R, 125);
    analogWrite(dvor_G, 125);
    analogWrite(dvor_B, 125);

    command = 0;
  }
  else if (command == 0x14) // turn ALL light on
  {
    shiftPic();  // lightup the world
    lightTimer = millis();
    lightDelay = 155000;
    setLightBri(225);
    command = 0;
  }
  else if (command == 0x23)     // dim the light while Ghera speaks after Thunder
  {
    lightTimer = millis();
    lightDelay = 68000;
    digitalWrite(extraLight, HIGH); //OFF
    setLightBri(10);
    command = 0;
  }
  else if (command == 0x30)     // dim the light while Ghera speaks after Shields done
  {
    lightTimer = millis();
    lightDelay = 90000;
    digitalWrite(extraLight, HIGH);
    setLightBri(20);
    command = 0;
  }
  else if (command == 0x31)    // wind for 10-15 secs
  {
    startWind = millis();
    windState = true;
    digitalWrite(wind, windState);
    command = 0;
  }
  else if (command == 0x41)     // moon-sun level
  {
    setLightBri(0);
    moonSun = true;
    shiftPic();  // start sun-moon show
    digitalWrite(extraLight, HIGH); //dark the room
    delay(200);
    analogWrite(oknoA_B, 150);
    analogWrite(oknoC_R, 150);
    command = 0;
  }
  else if (command == 0x42) // after moon-sun ended
  {
    // wind for 10-15 secs
    moonSun = false;
    setLightBri(0);
    lightTimer = millis();
    lightDelay = 4000;
    digitalWrite(flowerB, LOW);
    digitalWrite(flowerR, LOW);
    command = 0;
  }
  else if (command == 0x45)    // dim the light while Ghera speaks after Seals done
  {
    lightTimer = millis();
    lightDelay = 39000;
    digitalWrite(extraLight, HIGH); //dark the room
    setLightBri(25); // okna 50 % all
    command = 0;
  }
  else if (command == 0x55) // crystals ! lets change the light for this level
  {
    // lighAnimation();  call LIGHT ANIMATION SCRIPT  !!!!

    //temporal light while animation is missing
    digitalWrite(extraLight, LOW); //light up the room
    setLightBri(255); // okna 50 % all
    command = 0;
  }

  //---------------------------------------------------------------------------------------//

  //turn the room light off while Ghera speaks (time varies depend on level) sun-mooonshould disable auto on light
  if (lightTimer > 0) {
    if ((lightDelay + lightTimer) > millis()) {
      digitalWrite(extraLight, HIGH); //OFF
    } else {
      digitalWrite(extraLight, LOW); //ON
      lightTimer = 0;
      setLightBri(155); // okna after speak
    }
  }

  if (randomWind) randWind();
  else if (windState) // for stop wind after comand 0x80
  {
    if ((millis() - startWind ) > blowTime)
    {
      windState = false;
      digitalWrite(wind, windState);
    }
  }


}

void lighAnimation() {
  // LIGHT ANIMATION SCRIPT HERE  -- ENDING __ synced with sound from master !!!!
}

void receiveEvent(int howMany)
{
  if (Wire.available() > 0) // пройтись по всем до последнего
  {
    byte c = Wire.read();    // принять байт как символ
    if ( c > 0 ) command = c;
  }
}

void requestEvent()
{
  byte ans = 0x00;
  for (int i = 0; i < 3; i++)
  {
    if (crystStates[i]) ans |= (1 << i * 2);
  }
  Wire.write(ans);
}

/*

  bigKey = 1
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
// OBSOLETE
