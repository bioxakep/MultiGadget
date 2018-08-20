// Ago 09 2018
// Master SKY  - Mega 
// I2C master - linked to Motor_Controller & Lights_Controller
// RS-485 interface - Serial1 - link to Admin
// xBee - Serial3 - link to light_controller and World_prop 
// mp3_player - on Serial(TX)

#include <OneWire.h>
#include <ArdCPZ.h>
#include <DFPlayer_Mini_Mp3.h>
#include <SoftwareSerial.h>

int startRF  = 8;    // RFID key to start 
int underRF  = 9;    // RFID key to the underground door
int gateRF   = 10;   // RFID key to gate

ArdCPZ *cpz1;
ArdCPZ *cpz2;
ArdCPZ *cpz3;

int startBeacon = 499; bool startCard = false;
int underBeacon = 328; bool underDoor = false;
int gateBeacon  = 230; bool  gateDoor = false;


int balloIN  = 50;   // a.
int balloOUT = 44;   

int pressIN  = 48;   // b.
int pressOUT = 42;

int demetIN  = 26;   // d.
int demetOUT = 38;
int demetHD  = A1;

int vinemIN  = 24;   //e.     
int vinemOUT = 36;

int dioniIN  = 22;   //f.
int dioniOUT = 34;
int dioniHD1 = A2;   // piston
int dioniHD2 = A3;          

int hercuIN  = 7;    
int hercuHD  = A4;

int molniIN  = 52;   // g.
int molniOUT = 32;


int poseiIN  = 53;   // h.
int poseiOUT = 43;
int poseiHD  = A5;

int narciIN  = 51;   // i.  /// video seen
int narciOUT = 41;          /// launch video 

// lev 3

int noteIN   = 49;   // j.
int noteOUT  = 39;
int noteHD   = A6;   

int octopIN  = 47;   // k.
int octopOUT = 37; 

int afinaIN  = 27;   // m. 
int afinaOUT = 35;   // 

int afinaHD1 = A7; 
int afinaHD2 = A8;

/// level 4

int musesIN  = 23;   // o. to change their messages
int musesOUT = 31;   // correct pattern
int arphaIN  = 6;
int arphaHD  = A9;

// level 5
/// subRFID  
/// doorMAG  

int gorgoIN  = 28;   // A.
int gorgoOUT = 30; 
int gorgoHD  = A12; 

int minotIN  = 29;   // B.
int minotOUT = 45;       
int minotHD  = A10;

int zodiaIN  = 5;
int zodiaHD  = A10;

int flowrIN  = 25;
int flowrOUT = 33;   //n.

int gheraIN   = 4;
int gheraOUT  = 2;

int spare1 = 12;
int spare2 = 11;

int level = 0;

bool balloPASS = false;
bool pressPASS = false;
bool gatePASS  = false;


void setup() {
Serial.begin (9600);
Serial3.begin(9600);  //xbee
Serial1.begin(9600);  //RS-485 adapter

Serial.println("\nSKY Master v1 ");
Serial.println("17 Aug 2018");

  delay(100);
  mp3_set_serial(Serial);
  delay(100);
  mp3_set_volume (28);
  delay(100);
  mp3_play(1); // test startup sound
  delay(200);

Serial.println("\n--------------");


pinMode( balloIN, INPUT_PULLUP);
pinMode( balloOUT, OUTPUT);   

pinMode( pressIN, INPUT_PULLUP);
pinMode( pressOUT, OUTPUT);

pinMode( demetIN, INPUT_PULLUP);
pinMode( demetOUT, OUTPUT);
pinMode( demetHD , OUTPUT);

pinMode( vinemIN, INPUT_PULLUP);     
pinMode( vinemOUT , OUTPUT);    // click-lock on prop,

pinMode( dioniIN, INPUT_PULLUP);
pinMode( dioniOUT , OUTPUT);
pinMode( dioniHD1 , OUTPUT); // piston
pinMode( dioniHD2 , OUTPUT);     

pinMode( hercuIN, INPUT_PULLUP);
pinMode( hercuHD  , OUTPUT);

pinMode( molniIN, INPUT_PULLUP);
pinMode( molniOUT , OUTPUT);


pinMode( poseiIN, INPUT_PULLUP);
pinMode( poseiOUT , OUTPUT);
pinMode( poseiHD  , OUTPUT);

pinMode( narciIN, INPUT_PULLUP);
pinMode( narciOUT , OUTPUT);     /// launch video 

pinMode( noteIN, INPUT_PULLUP);
pinMode( noteOUT, OUTPUT);
pinMode( noteHD , OUTPUT);

pinMode( octopIN, INPUT_PULLUP);
pinMode( octopOUT , OUTPUT);    // click-lock on prop

pinMode( afinaIN, INPUT_PULLUP); 
pinMode( afinaOUT , OUTPUT); 

pinMode( afinaHD1 , OUTPUT);
pinMode( afinaHD2 , OUTPUT);

pinMode( musesIN, INPUT_PULLUP);
pinMode( musesOUT , OUTPUT);

pinMode( arphaIN, INPUT_PULLUP);
pinMode( arphaHD  , OUTPUT);

pinMode( gorgoIN, INPUT_PULLUP);
pinMode( gorgoOUT, OUTPUT);
pinMode( gorgoHD,  OUTPUT);  //click

pinMode( minotIN, INPUT_PULLUP);
pinMode( minotOUT , OUTPUT);   
pinMode( minotHD  , OUTPUT); // click

pinMode( zodiaIN, INPUT_PULLUP);
pinMode( zodiaHD  , OUTPUT); //click

pinMode( flowrIN, INPUT_PULLUP);
pinMode( flowrOUT , OUTPUT);

pinMode( gheraIN, INPUT_PULLUP);
pinMode( gheraOUT , OUTPUT);


  cpz1 = new ArdCPZ(startRF);
  cpz2 = new ArdCPZ(underRF);
  cpz3 = new ArdCPZ(gateRF);

bool startCard = false;
bool underDoor = false;
bool gateDoor  = false;

checkInputs();
openLocks();
}

void loop() {

 if (level == 0) {
  // wait for prestart signal ( any RFID from startRF )
  // if received send command to Motor_controller and Light_controller
  mp3_play(2); // prestart message
  level  = 10;
 }

 if (level = 10) {
  // wait for start signal ( any RFID from startRF )
  // start the game
  // signal to Light_controller > wind and lights
  mp3_play(3); // start message
  level = 20;
 }

 if (level = 20) {
  // wait for signal from balloIN
  // send signal to motor_controller >rollUp and light_controller  >lights and stop wind 
  level = 30;
 }

 if (level = 30) {
  // wait for signal from pressIN  -- players get gateRFIDkey
  level = 40;
 }

 if (level = 40) {
  // wait for signal from gateRFID
  // send signal to motor_controller >openGate > gheraLevel = 1 > and start Ghera > open HD1 
  level = 50;
  signal2ghera();
 }

 if (level = 50) { // all events may be done in any order/sequence

  // ---------molnii--------------
  // posei > motor_controller falling column > players get first part molnii
  // demet - grape grow   - vineMkr  > players get vine bottle for dionis
  // dionis(1)  HD1 > players gets second part molnii
  // hercu > players gets third part of molnii
  // narci > only help information for players
  //if all molnii are done >   // gheraLevel = 2 >  speaks > opend HD2 (shields)    
  signal2ghera();

  
   //-------- shields ------
   // afina done > open afinaHD ???????????????????  > players get escu1
   // octop done > players get escu2
   // note > open HD ?????(motor_controller - cloud down) done > players get escu3
   //if all shields are in ghera place > gheraLevel = 3 > gera speaks - open HD3 (seals)
   signal2ghera();


   //---------seals --------
   // if world  done > players get seal1
   // dionis(2) - HD2 - cold heart if all done > players get seal 2 
   // muses - start - arpha if  done > players get seal 3
   //if all seals are in ghera place  > gheraLevel = 4 > ghera speaks > column up > open HD4 (key for underground)
   signal2ghera();
 
    //---------cristals------ 
    // if zodia done > players get cryst1
    // if gorgo done > players get cryst2
    // if minot done > players get cryst3
    //if all crystals are in place > give shoe  >>> final
    level = 100;
    signal2ghera();
 }

 if (level == 100) {
   // game over
 }
}
