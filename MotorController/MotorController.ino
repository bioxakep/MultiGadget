// Motor Comtroller v1
// 13 AGO 2018
// 21 AGO 2018 adjusted - I2C enabled - xBee deleted
// 12 sep 2018 remote control temp sequence created, auto column and auto grape tested
// 17 NOV 2018 automatic sequences corrected 
// 18 NOV 2018 curtain1 and 2 added, megaColumn
#include "Wire.h" // I2C
#include <DFPlayer_Mini_Mp3.h>

int remote1   = 31;
int remote2   = 35;
int remote3   = 39;
int remote4   = 43;

int gateUP    = A15;   //was A2 moved SSR to ?????????????????????
int gateTOP   = A11;  // not used - FREE

int columnDN  = A12;
int columnUP  = A13;
int columnTOP = A9;
int columnBOT = A8;
int columnOOO = A10;

int grapeUP   = 27;
int grapeDN   = 29;
int grapeTOP  = 7;
int grapeBOT  = 6;
int grapeOOO  = 5;

int cloudUP   = 23;
int cloudDN   = 25;
int cloudTOP  = 4;   
int cloudOOO  = 3; // not used

int curtain1UP   = 47;
int curtain1DN   = 45;

int curtain2UP   = A1;
int curtain2DN   = A2;

int curtain1TOP   = A11;
int curtain2TOP   = A11;

int underDoor = A7;

int megaColumn = 22;

int poseiPump = A4;  // relay pending
int poseiValv = A5;  // relay pending
int poseiLock = A6;  // relay pending

byte command = 0;
int  thisI2CAddr = 21;

bool  grapeOnTop   = false;
bool  grapeOnBot   = false;
bool  columnOnTop  = false;
bool  columnOnBot  = false;

bool  cloudOnTop   = false;
bool  cloudOnBot   = true;

bool  curtain1OnTop = true;
bool  curtain1OnBot = false;
bool  curtain2OnTop = true;
bool  curtain2OnBot = false;

int   led           = 13;

void setup() {
  Serial.begin(9600);
  Serial3.begin(9600);
  Wire.begin(thisI2CAddr);
  Wire.onReceive(receiveEvent);
  mp3_set_serial(Serial3);
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

  pinMode(gateTOP, INPUT_PULLUP);  // not used
  
  pinMode(columnTOP, INPUT_PULLUP);
  pinMode(columnBOT, INPUT_PULLUP);
  pinMode(columnOOO, INPUT_PULLUP);

  pinMode(grapeTOP, INPUT_PULLUP);
  pinMode(grapeBOT, INPUT_PULLUP);
  pinMode(grapeOOO, INPUT_PULLUP);

  pinMode(cloudTOP, INPUT_PULLUP);
  pinMode(cloudOOO, INPUT_PULLUP);

  pinMode(poseiPump,  OUTPUT);
  pinMode(poseiValv,  OUTPUT);
  pinMode(poseiLock,  OUTPUT);

  digitalWrite(gateUP,     LOW);

  digitalWrite(megaColumn, LOW);

  digitalWrite(columnDN,   LOW); 
  digitalWrite(columnUP,   LOW); 

  digitalWrite(grapeUP,    LOW);
  digitalWrite(grapeDN,    LOW);

  digitalWrite(curtain1UP,  HIGH);
  digitalWrite(curtain1DN,  HIGH);

  digitalWrite(curtain2UP,  HIGH);
  digitalWrite(curtain2DN,  HIGH);
  
  digitalWrite(cloudUP,    HIGH);
  digitalWrite(cloudDN,    HIGH);

  digitalWrite(underDoor,  LOW);

  digitalWrite(poseiValv,  HIGH);   // unpowered valve dont let water get out of the vault
  digitalWrite(poseiPump,  HIGH);
  delay(1000 );                    /// some time to fill the poseidon vault with water
  digitalWrite(poseiPump,  HIGH);
  digitalWrite(poseiLock,  HIGH);   // lock the cover of the vault while if full of water

  Serial.println("Motor Controller \n21_AGO_2018 - 18_NOV \nHardware  = Mega\n");
  checkInputs();
  digitalWrite(led, LOW);
  Serial.println("\nReady.");
}

void loop() {

remoteControl();

  // receives command from master via i2c
  if (command == 0x10) 
  { 
    Serial.println("Initial command from master , at first START...");
   // cloudUpCom();
   // grapeUpCom();
   // columnDownCom();
    Serial.println("Initialize command Done.");
  }
  else if (command == 0x12)  
  {  // поднимаем шторы (их две)
     Serial.println("curtainUP command from master");
     curtain1UpCom();
     Serial.println("curtainUP command done");
  }
  else if (command == 0x14)
  { 
    Serial.print("\nGate UP command from master...");
    gateUpCom();
    Serial.println("Done.");
  }
  else if(command == 0x20)
  { // Тайник посейдона
    poseiVaultOpen();
  }
  else if(command == 0x21)
  {//sendToSlave(motorConAddr, 0x21); // Column Up
    Serial.print("\nColumn Up command from master...");
    columnUpCom();
    Serial.println("Done.");
  }
  else if(command == 0x22)
  {//sendToSlave(motorConAddr, 0x22); // send signal to motor_controller > grapeGrow
    Serial.print("\nGrape Dn command from master");
    grapeDownCom();
    Serial.println("Done.");
  }
  else if (command == 0x31)
  { //cloudDown
    Serial.println("\nCloud Down command from master...");
    cloudDownCom();
  }
  else if (command == 0x51)
  { //Under doors Open
   Serial.println("\nunderDoor command from master...");
   digitalWrite(underDoor, HIGH);
  }
  else if (command == 0x61)
  { //Mega Columns goes UP
   Serial.println("\n/Mega Columns command from master...");
   digitalWrite(megaColumn, HIGH);
  }

  command = 0;
}

void receiveEvent(int howMany) 
{
  if (Wire.available() > 0) // пройтись по всем до последнего
  { 
    command = Wire.read();    // принять байт как символ
  }
}
