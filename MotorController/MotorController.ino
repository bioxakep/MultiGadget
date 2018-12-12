// Motor Comtroller v1
// 13 AGO 2018
// 21 AGO 2018 adjusted - I2C enabled - xBee deleted
// 12 sep 2018 remote control temp sequence created, auto column and auto grape tested
// 17 NOV 2018 automatic sequences corrected 
// 18 NOV 2018 curtain1 and 2 added, megaColumn
// 27 NOV 2018 uWhiteLitr added
//  1 DEC 2018 wire speed at 10000
//  5 DEC 2018 remote shifted,  cloud included
//  7 DEC 2018 remote turned OFF permanently
// 11 dec 2018  adjusted timing

#include "Wire.h" // I2C
#include <DFPlayer_Mini_Mp3.h>

int remote1   = 31;
int remote2   = 35;
int remote3   = 39;
int remote4   = 43;

int gateUP    = A15;   //was A2 moved SSR to ?????????????????????

int columnDN  = A12;
int columnUP  = A13;
int columnTOP = A9;
int columnBOT = A8;
int columnOOO = A10;

int grapeUP   = 29;
int grapeDN   = 27;
int grapeTOP  = 7;
int grapeBOT  = 6;
//int grapeOOO  = 5;

int cloudUP   = 23;
int cloudDN   = 25;
int cloudTOP  = 4;   
//int cloudOOO  = 3; // not used

int curtain1UP   = A4;
int curtain1DN   = A5;

int curtain2UP   = A1;
int curtain2DN   = A2;

int curtain1TOP   = 5;
int curtain2TOP   = 3;

int underDoor = A7;
int uWhiteLite= A11;  

int megaColumn = 22;

int poseiPump = 28;  // SSR relay 110v
int poseiValv = 26;  // relay 
int poseiLock = 24;  // relay 

byte command = 0;
int  thisI2CAddr = 21;

bool  grapeOnTop    = false;
bool  grapeOnBot    = false;
bool  columnOnTop   = false;
bool  columnOnBot   = false;

bool  cloudGoesUp   = false;
bool  cloudOnTop    = false;
long  cloudTimer    = 0;
long  cloudDelay    = 45000;

bool  curtain1OnTop = true;
bool  curtain1OnBot = false;
bool  curtain2OnTop = true;
bool  curtain2OnBot = false;
bool  curtainMove   = false;
int   led           = 13;

void setup() {
  Serial.begin(9600);
  Serial3.begin(9600); // df_player
  Wire.begin(thisI2CAddr);
  Wire.setClock(10000);
  Wire.onReceive(receiveEvent);
  mp3_set_serial(Serial3);
  delay(100);
  mp3_set_volume (25);
  delay(100);

  
  pinMode(remote1, INPUT_PULLUP);
  pinMode(remote2, INPUT_PULLUP);
  pinMode(remote3, INPUT_PULLUP);
  pinMode(remote4, INPUT_PULLUP);
  pinMode(led,OUTPUT);
  digitalWrite(led,HIGH);
  pinMode(gateUP, OUTPUT);    // just close contact momentary

  pinMode(megaColumn, OUTPUT); 

  pinMode(columnDN, OUTPUT); 
  pinMode(columnUP, OUTPUT); 

  pinMode(grapeUP,  OUTPUT);
  pinMode(grapeDN,  OUTPUT);

  pinMode(curtain1UP,  OUTPUT);
  pinMode(curtain1DN,  OUTPUT);

  pinMode(curtain2UP,  OUTPUT);
  pinMode(curtain2DN,  OUTPUT);

  pinMode(curtain1TOP, INPUT_PULLUP);  
  pinMode(curtain2TOP, INPUT_PULLUP);  
  
  pinMode(cloudUP,  OUTPUT);
  pinMode(cloudDN,  OUTPUT);

  pinMode(underDoor, OUTPUT);
  pinMode(uWhiteLite, OUTPUT);

 // pinMode(gateTOP, INPUT_PULLUP);  // not used
  
  pinMode(columnTOP, INPUT_PULLUP);
  pinMode(columnBOT, INPUT_PULLUP);
  pinMode(columnOOO, INPUT_PULLUP);

  pinMode(grapeTOP, INPUT_PULLUP);
  pinMode(grapeBOT, INPUT_PULLUP);

  pinMode(cloudTOP, INPUT_PULLUP);

  pinMode(poseiPump,  OUTPUT);
  pinMode(poseiValv,  OUTPUT);
  pinMode(poseiLock,  OUTPUT);

  digitalWrite(gateUP,     LOW);

  digitalWrite(megaColumn,  LOW);

  digitalWrite(columnDN,    LOW); 
  digitalWrite(columnUP,    LOW); 

  digitalWrite(grapeUP,     LOW);
  digitalWrite(grapeDN,     LOW);

  digitalWrite(curtain1UP,  HIGH);
  digitalWrite(curtain1DN,  HIGH);

  digitalWrite(curtain2UP,  HIGH);
  digitalWrite(curtain2DN,  HIGH);
  
  digitalWrite(cloudUP,     HIGH);
  digitalWrite(cloudDN,     HIGH);

  digitalWrite(underDoor,   LOW);
  digitalWrite(uWhiteLite,  HIGH);

  digitalWrite(poseiValv,   LOW);   // unpowered valve dont let water get out of the vault
  digitalWrite(poseiPump,   HIGH);
  delay(1000 );                    /// some time to fill the poseidon vault with water
  digitalWrite(poseiPump,   LOW);
  digitalWrite(poseiLock,   LOW);   // lock the cover of the vault while if full of water

  Serial.println("Motor Controller \n21_AGO_2018 - 11_DEC_2018 \nHardware  = Mega\n");
  checkInputs();
  digitalWrite(led, LOW);
  Serial.println("\nReady.");

//  curt1TOP  = HIGH  ,  curt2TOP  = LOW  both on top
if ( digitalRead(curtain1TOP)==HIGH || digitalRead(curtain2TOP)==LOW ) curtainMove = false; else curtainMove = true; 

if ( digitalRead(cloudTOP)==LOW ) cloudOnTop = true; else cloudOnTop = false; 

columnUpCom();
grapeDownCom();
cloudDownCom();

// mp3_play(999); // test startup sound

}

void loop() {

 if (cloudTimer > 0 && ((millis() - cloudTimer) > cloudDelay)) 
    {
      digitalWrite(cloudDN, HIGH);
      digitalWrite(cloudUP, HIGH);
      cloudOnTop = false;
      cloudTimer = 0;
      Serial.println("cloud is moved to Bottom position");
    }
 
 if (cloudGoesUp == true )
    {
     if (digitalRead(cloudTOP)==LOW) {
      digitalWrite(cloudUP, HIGH);
      digitalWrite(cloudDN, HIGH);
      cloudOnTop = true;
      cloudGoesUp = false; 
      Serial.println("cloud is moved to Top position");
     }
    }


 remoteControl();

  // receives command from master via i2c
  if (command == 0x10) 
  { 
    Serial.println("Initial command from master , at first START...");
  // megaColumnUpCom();
   grapeUpCom();
   columnDownCom();
   curtainDownCom();
   //cloudUpCom();
   // lock all locks
    Serial.println("Initialize command Done.");
    mp3_play(999);
   command = 0;
  }
  else if (command == 0x11)  
  {  // поднимаем cloud
     Serial.println("cloudUP command from master");
     cloudUpCom();
     //mp3_play(999);
     command = 0;
  }
  else if (command == 0x12)  
  {  // поднимаем шторы (их две)
     Serial.println("curtainUP command from master");
     curtainUpCom();
     Serial.println("curtainUP command done");
     mp3_play(999);
     command = 0;
  }
  else if (command == 0x14)
  { 
    digitalWrite(gateUP, HIGH);
    Serial.print("\nGate UP command from master...");
    Serial.println("Done.");
    mp3_play(999);
    command = 0;
 }
  else if(command == 0x20)
  { // Тайник посейдона
    digitalWrite(gateUP, HIGH);
    poseiVaultOpen();
    digitalWrite(gateUP, LOW);
    mp3_play(999);
    command = 0;

  }
  else if(command == 0x21)
  {//sendToSlave(motorConAddr, 0x21); // Column Up
    columnUpCom();
    Serial.print("\nColumn Up command from master...");
    mp3_play(999);
    Serial.println("Done.");
   command = 0;
  }
  else if(command == 0x22)
  { //sendToSlave(motorConAddr, 0x22); // send signal to motor_controller > grapeGrow
    Serial.print("\nGrape Dn command from master Start");
    grapeDownCom();
    Serial.println("Grape Dn command from master Done.");
    mp3_play(999);
    command = 0;
 }
  else if (command == 0x31)
  { //cloudDown
    Serial.println("\nCloud Down command from master...");
    //mp3_play(2); // wind sound
    mp3_play(999);
    cloudDownCom();
    command = 0;
  }
  else if (command == 0x51)
  { //Under doors Open
   Serial.println("\nunderDoor command from master...");
   digitalWrite(megaColumn, HIGH);
   command = 0;
  }
  else if (command == 0x53)
  { //underground open
   mp3_play(1);
   command = 0;
  }
  else if (command == 0x55)
  { //Zodiak gone
   Serial.println("\nZodiak gone...");
   mp3_play(2);
   command = 0;
  }
  else if (command == 0x61)
  { //Mega Columns goes UP
   Serial.println("\n/Mega Columns command from master...");
   megaColumnUpCom();
   command = 0;
  }
  else if (command == 0x99)
  { //game over stop the music
   Serial.println("\nGame over...");
   mp3_stop();
   command = 0;
  }

}

void receiveEvent(int howMany) {
  if (Wire.available() > 0) // пройтись по всем до последнего
  { 
    command = Wire.read();    // принять байт как символ
  }
}
