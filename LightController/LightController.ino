// Light Controller v1 (beta)
// 17 AGO 2018
// I2C enabled - slave for master // xBee removed - receives commands from Master and World
// 23 AGO 2018 flowers added

// bright on all windows
// red on selected window
// blue on selected window
// green on all windows
// dark on inactive windows

/// PWM color control for windows
#include "Wire.h" // I2C

int oknoA_R   = 12;
int oknoA_G   = 11;
int oknoA_B   = 10;

int oknoB_R   =  9;
int oknoB_G   =  8;
int oknoB_B   =  7;

int oknoC_R   =  6;
int oknoC_G   =  5;
int oknoC_B   =  4;

int dvor_W    = 46;
int dvor_R    = 45;
int dvor_G    = 3;
int dvor_B    = 2;

int wind      = A8; // AC relay to control Wind blower
int underlight = A2; // relay to control underground lighting (not used in game but remotely by operator)
int extralight = A7; // additional light (not implemented yet) but available on hardware as 5A DC_Relay
int cryst1    = 23;
int cryst2    = 24;
int cryst3    = 25;

int flowerR   = 53;
int flowerB   = 51;

byte command = 0;
int thisI2CAddr = 20;

boolean windState = false;
boolean randomWind = false;
int minWindPause = 100;
int maxWindPause = 900;
int minWindTime = 1500;
int maxWindTime = 3000;
unsigned long windTime = 0;
unsigned long windPause = 0;
unsigned long startWind = 0;
unsigned long stopWind = 0;

void setup() 
{
  Serial.begin(9600);
  Wire.begin(thisI2CAddr);
  Wire.onReceive(receiveEvent);
  
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
  pinMode(underlight, OUTPUT);
  pinMode(extralight, OUTPUT);

  pinMode(flowerR, OUTPUT);
  pinMode(flowerB, OUTPUT);

  digitalWrite(flowerR, LOW);
  digitalWrite(flowerB, LOW);
  digitalWrite(wind,    LOW);
  digitalWrite(underlight, HIGH);
  digitalWrite(extralight, LOW);

  pinMode(cryst1, INPUT_PULLUP);
  pinMode(cryst2, INPUT_PULLUP);
  pinMode(cryst3, INPUT_PULLUP);

  Serial.println("\nLight Controller v1  \n21_AGO_2018 ");
  Serial.println("Hardware = Mega\n");

  testing();

  Serial.println("Crystal 1  = " + String(digitalRead(cryst1)));
  Serial.println("Crystal 2  = " + String(digitalRead(cryst2)));
  Serial.println("Crystal 3  = " + String(digitalRead(cryst3)));
  randomSeed(A0);
}

void loop() {
  
  if(command > 0) Serial.println("Command = " + String(command));

  if(command == 0x01) lightOff(); // Выключить весь свет
  else if(command == 0x02) randomWind = true;
  else if(command == 0x03) // Baloon passed
  {
    randomWind = false;
    windState = false;
    digitalWrite(wind, windState);
  }
  
  command = 0;


  if(randomWind) randWind();
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
  if (Wire.available()) // пройтись по всем до последнего
  { 
    byte c = Wire.read();    // принять байт как символ
    if(c > 0x00 && c < 0x20) command = c;
  }
}

