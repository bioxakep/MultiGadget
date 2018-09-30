// Motor Comtroller v1
// 13 AGO 2018
// 21 AGO 2018 adjusted - I2C enabled - xBee deleted
// 12 sep 2018 remote control temp sequence created, auto column and auto grape tested
#include "Wire.h" // I2C
#include <DFPlayer_Mini_Mp3.h>

int remote1   = 31;
int remote2   = 35;
int remote3   = 39;
int remote4   = 43;

int gateUP    = A2;
int gateTOP   = A11;

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
int cloudOOO  = 3;

int curtainUP   = 47;
int curtainDN   = 45;

int mainDoor  = A1;
int underDoor = A7;

int poseiPump = A4;
int poseiValv = A5;
int poseiLock = A6;

byte command = 0;
int thisI2CAddr = 21;
//int courtainUP = not implemented yet
//int courtainDN = not implemented yet

bool  grapeOnTop = false;
bool  grapeOnBot = false;
bool  columnOnTop = false;
bool  columnOnBot = false;

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

  pinMode(gateUP, OUTPUT);    // just close contact for 15 seconds to complete the action or until gateTOP is LOW

  pinMode(columnDN, OUTPUT);  // close contact for xx seconds or count columnOOO till 300
  pinMode(columnUP, OUTPUT);  // close contact for xx seconds or count columnOOO till 300

  pinMode(grapeUP,  OUTPUT);
  pinMode(grapeDN,  OUTPUT);

  pinMode(curtainUP,  OUTPUT);
  pinMode(curtainDN,  OUTPUT);
  
  pinMode(cloudUP,  OUTPUT);
  pinMode(cloudDN,  OUTPUT);

  pinMode(mainDoor,  OUTPUT);
  pinMode(underDoor, OUTPUT);

  pinMode(gateTOP, INPUT_PULLUP);

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

  digitalWrite(mainDoor, LOW);
  digitalWrite(underDoor, LOW);

  digitalWrite(poseiValv, LOW);   // unpowered valve dont let water get out of the vault
  digitalWrite(poseiPump, HIGH);
 /// delay(10000);                    /// some time to fill the poseidon vault with water
  digitalWrite(poseiPump, LOW);
  digitalWrite(poseiLock, HIGH);   // lock the cover of the vault while if full of water

  Serial.println("\nMotor Controller v1  \n21_AGO_2018 ");
  Serial.println("Hardware = Mega\n");
  checkInputs();
}

void loop() {

  if (digitalRead(remote1)==LOW &&  grapeOnTop == false) {            // manual  control by radio remote control
    Serial.println("\nRemote control (1) grape up");
    digitalWrite(grapeUP, HIGH);
    while(  digitalRead(grapeTOP)==LOW ) {;}
    Serial.println("\ngrape on TOP");
    grapeOnTop = true; 
    grapeOnBot = false; 
    // command = 0x02;
    //grapeUp();
  } else    digitalWrite(grapeUP, LOW);
  
  if (digitalRead(remote2)==LOW &&  grapeOnBot == false) {           // manual  control by radio remote control
    Serial.println("\nRemote control (2) grape down");
    digitalWrite(grapeDN, HIGH);
    while(  digitalRead(grapeBOT)==LOW ) {;}
    Serial.println("\nGrape is down"); 
    grapeOnTop = false;
    //command = 0x04;
    //grapeDown();
  } else     digitalWrite(grapeDN, LOW);

//Column UP auto
  if (digitalRead(remote3)==LOW &&  columnOnTop == false) {            // manual  control by radio remote control
    Serial.println("\nRemote control (3) column up");
    digitalWrite(columnUP, HIGH);
    while(  digitalRead(columnTOP)==LOW ) {;}
    Serial.println("\nColumn on TOP");
    columnOnTop = true; 
    columnOnBot = false;   
    // command = 0x0x;
  } else    digitalWrite(columnUP, LOW);

  if (digitalRead(remote4)==LOW &&  columnOnBot == false) {           // manual  control by radio remote control
    Serial.println("\nRemote control (4) column down");
    digitalWrite(columnDN, HIGH);
    while(  digitalRead(columnBOT)==LOW ) {;}
    columnOnBot = true; 
    columnOnTop = false;  
    //command = 0x0x;
    
  } else     digitalWrite(columnDN, LOW);

  // receives command from master via i2c
  if (command == 0x10)
  { 
    Serial.print("\nOpen the gate start...");
    cloudUp();
    grapeUp();
    columnUp();
    Serial.println("Done.");
  }

  if (command == 0x20)
  { 
    Serial.print("\ncurtainUP start...");
    digitalWrite(curtainUP, HIGH);
    delay(200);
    digitalWrite(curtainUP, LOW);
    Serial.println("Done.");
  }

  //level 30 without command
  
  if (command == 0x40)
  { 
    Serial.print("\nGate UP start...");
    gateUp();
    Serial.println("Done.");
  }

  if (command == 0x50)
  {
    Serial.print("\nGrape UP start...");
    grapeUp();
    Serial.println("Done.");
  }
  
  if (command == 0x51) // Trident
  {
    Serial.print("\nColumn Down start...");
    columnDown();
    Serial.println("Done.");
  }

  if (command == 0x52) // Grape Grow
  {
    Serial.print("\nColumn Down start...");
    grapeUp();
    Serial.println("Done.");
  }
/*
  if (command == 0x07) { // column up  - top sensor available
    Serial.println("\nColumn UP start...");
    columnUp();
    
    Serial.println("\nColumn UP done");
  }

  if (command == 0x08) { // open underground door
    Serial.println("\nunderground Door OPEN");
    digitalWrite(underDoor, LOW);
  }

  if (command == 0x09) { // open main door
    Serial.println("\nmain Door OPEN");
    digitalWrite(mainDoor, LOW);
  }

  if (command == 0xFF) { // poseidon - lets open poseidon vault
    Serial.println("\nPoseidon vault OPENING...");
    poseiVaultOpen();
    Serial.println("\nPoseidon vault OPEN");
    
  }
  */
  command = 0;
}

void receiveEvent(int howMany) 
{
  if (Wire.available()) // пройтись по всем до последнего
  { 
    byte c = Wire.read();    // принять байт как символ
    if(c > 0x00 && c < 0x20) command = c;
  }
}

