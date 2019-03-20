//december 06 all inputs corrected


#include <SimpleTimer.h>
#include "FastLED.h"
#define NUM_LEDS 160
#define DATA_PIN A1
CRGB leds[NUM_LEDS];
SimpleTimer timer;

#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>

/// ---DIRECT INPUTS ----
int emerA     = 15; // ORA
int emerB     = 14; // GRN
int StopHU    = 52; // BK
int roof      = 3;  // BRW - GRN - GRN


////-- thru Addon OptoCoupler
int remote    = A0; // WHT  
int tubes     = A15; // BRW


///  --- OPTO INPUTS ----
int CharSigA  = 7;  // GRN
int VacSig    = 5;  // BK
int leakSig   = 6;  // WHT-GRN - ORA/thn
int CharSigB  = 4;  // RD 

// --- extra output ----
int airSSR    = 36;  // BK  110v
int airLOW    =  2;  // WH  110v

/// --- CLOCK ----
byte segmentClock = 51; // RED - BRW  
byte segmentLatch = 53; // WHT - GRY
byte segmentData  = 50; // GRN - BLU 

///  --- IR LEDS ORIGINAL/PHYSICAL ----
//t irled[6]  = { 26,  28,  30,  24,  32,  34 };  
//t charg[6]  = { 48,  46,  44,  42,  40,  38 };  // hall sensors
///  ---IR LEDS ----
// se muestra el ( 3,   5,   2,   4,   1,   6 )
int irled[6]  = { 30,  32,  28,  24,  26,  34 };  
int charg[6]  = { 44,  40,  46,  42,  48,  38 };  // hall sensors



/// ---- SPARE OPTO RES-----
int spareOUT_optoRES1   = 8; 
int toLeak     = 9;  // optoresistor GRN/RED

// ---- VIDEO PLAY ONE ------  UPPER SCR   ---  LEFT PLAYER
int videoA1    = 12;  // GRN - BRW
int videoA2    = 11;  // WHT - ORA
int videoA3    = 10;  // RED - GRN

// ---- VIDEO PLAY TWO ------ LOWER SCR    --- RIGHT PLAYER
int videoB1    = 16;  // GRN - BRW
int videoB2    = 17;  // WHT - ORA
int videoB3    = 18;  // WHT - XXX  ???
int videoB4    = 19;  // WHT - XXX  ???

///-----TRANSISTORS-----
int  vac_Out   = A2;  // BLK
int  stopped   = A3;  // BRW
int  seatAfd   = A4;  // WHT
int  seatBfd   = A5;  // ORA 
int   bluTop   = A6;  // YEL game
int   redTop   = A7;  // GRN game
int  toCharA   = A10; // BRW  -12v
int  toCharB   = A11; // WT   -12v
int  operOut   = A12; // SIGNAL THE LEVEL ON OPERATOR DISPLAY + time left  

int seatA      = A13;  // dt-bps 2200 - BLU    HA
int seatB      = A14;  // dt-bps 2200 - WHT

//int          = A15;  // BLU


///------RELAYS ---------

int hide1      = 37;  // rel 1
int hide2      = 35;  // rel 2
int hide3      = 33;  // rel 3
int hide4      = 31;  // rel 4
int hideMas    = 29;  // rel 5
int HDlight    = 27;  // rel 6
int door       = 25;  // rel 7
int sw12v      = 23;  // rel 8

int fans       = 39;  // rel 9
int focus      = 41;  // rel 10
int LeakMas    = 43;  // rel 11
int handlockA  = 45;  // rel 12
int handlockB  = 47;  // rel 13
int roofcolR   = 49;  // rel 14 // unused - free
int blu24v     = 22;  // rel 15 
int red24v     = A9;  // rel 16 


////----- variables ------
int  number    = 60;
int  level     = 600;
int  i         = 0;
int  lastlevel = 0;
long inicio    = 0;
bool chargingGame = false;
bool tubesPass    = false; // pressure check on right hose
bool chargeGame   = false; 

////  ------var for charging game --------------
int bar          = 0;
int l            = 0;
int actual [6]   = {  3,  5,  2,  4,   1,   6 };
int chBlock[6]   = { 26, 51, 66, 92, 132, 160 };
int chCharged[6] = {  0,  0,  0,  0,   0,   0 };
int actBlock     = 0;

void setup() {
  Serial.begin(9600);
  mp3_set_serial(Serial);  
  mp3_set_volume(20);  
  delay(50);

  LEDS.addLeds<WS2812,A1,GRB>(leds, NUM_LEDS);
  FastLED.setBrightness( 20 );
//  apagarTodo();

  pinMode (remote    , INPUT_PULLUP);
  pinMode (tubes     , INPUT_PULLUP);
  pinMode (CharSigA  , INPUT_PULLUP);
  pinMode (VacSig    , INPUT_PULLUP);
  pinMode (leakSig   , INPUT_PULLUP);
  pinMode (CharSigB  , INPUT_PULLUP);
  pinMode (roof      , INPUT_PULLUP);
  
  pinMode (StopHU    , INPUT_PULLUP);
  
  pinMode (emerA     , INPUT_PULLUP);
  pinMode (emerB     , INPUT_PULLUP);
  
  pinMode (seatA     , INPUT_PULLUP);
  pinMode (seatB     , INPUT_PULLUP);

  pinMode(segmentClock,  OUTPUT);
  pinMode(segmentData ,  OUTPUT);
  pinMode(segmentLatch,  OUTPUT);

  pinMode(toLeak,        OUTPUT);

  pinMode(hide1,     OUTPUT);  // rel 1
  pinMode(hide2,     OUTPUT);  // rel 2
  pinMode(hide3,     OUTPUT);  // rel 3
  pinMode(hide4,     OUTPUT);  // rel 4
  pinMode(hideMas,   OUTPUT);  // rel 5
  pinMode(HDlight,   OUTPUT);  // rel 6
  pinMode(door,      OUTPUT);  // rel 7
  pinMode(sw12v  ,   OUTPUT);  // rel 8

  pinMode(fans,      OUTPUT);  // rel 9
  pinMode(focus,     OUTPUT);  // rel 10
  pinMode(LeakMas,   OUTPUT);  // rel 11
  pinMode(handlockA, OUTPUT);  // rel 12
  pinMode(handlockB, OUTPUT);  // rel 13
  pinMode(roofcolR , OUTPUT);  // rel 14
  pinMode(blu24v ,   OUTPUT);  // rel 15
  pinMode(red24v,    OUTPUT);  // rel 16

  pinMode(  vac_Out, OUTPUT);   // BLK
  pinMode(  stopped, OUTPUT);   // BRW
  pinMode(  seatAfd, OUTPUT);   // WHT  
  pinMode(  seatBfd, OUTPUT);   // ORA 
  pinMode(   bluTop, OUTPUT);   // YEL 
  pinMode(   redTop, OUTPUT);   // GRN
  pinMode(  toCharA, OUTPUT);   //  
  pinMode(  toCharB, OUTPUT);   // 
//  pinMode(  , OUTPUT);   // 

  pinMode(   airSSR, OUTPUT);   // BK
  pinMode(   airLOW, OUTPUT);   // BK

// ---- VIDEO PLAY ONE ------
  pinMode(videoA1, OUTPUT);    // RED
  pinMode(videoA2, OUTPUT);    // WHT
  pinMode(videoA3, OUTPUT);    // GRN

// ---- VIDEO PLAY TWO ------
  pinMode(videoB1, OUTPUT);    // GRN
  pinMode(videoB2, OUTPUT);    // WHT
  pinMode(videoB3, OUTPUT);    // WHT
  pinMode(videoB4, OUTPUT);    // WHT


// ---- VIDEO PLAY ONE ------
digitalWrite(videoA1, LOW);    // RED
digitalWrite(videoA2, LOW);    // WHT
digitalWrite(videoA3, LOW);    // GRN

// ---- VIDEO PLAY TWO ------
digitalWrite(videoB1, LOW);    // GRN
digitalWrite(videoB2, LOW);    // WHT
digitalWrite(videoB3, LOW);    // WHT
digitalWrite(videoB4, LOW);    // WHT


  digitalWrite(airSSR, LOW); // shut off
  digitalWrite(airLOW, LOW); // shut off

  digitalWrite(segmentClock, LOW);
  digitalWrite(segmentData,  LOW);
  digitalWrite(segmentLatch, LOW);
  digitalWrite(toLeak,       LOW);

  digitalWrite(hide1,      HIGH); // rel 1 hide 3 HIGH = CLOSED AND NO LIGHT
  digitalWrite(hide2,      HIGH); // rel 2 hide 4
  digitalWrite(hide3,      HIGH); // rel 3 hide 1
  digitalWrite(hide4,      HIGH); // rel 4 hide 2
  digitalWrite(hideMas,    HIGH); // rel 5 hide +
  digitalWrite(HDlight,    HIGH); // rel 6 HIGH = ON, Head Fans and lights
  digitalWrite(door,       HIGH); // rel 7 door LOW = OPEN
  digitalWrite(sw12v  ,    HIGH); // rel 8 sw lights HIGH = ON
  
  digitalWrite(fans,       LOW);   // rel 9  fans  = ON ???
  digitalWrite(focus,      HIGH);  // rel 10 LOW = ON, small light
  digitalWrite(LeakMas,    HIGH);  // rel 11  OFF
  digitalWrite(handlockA,  LOW);   // rel 12  closed on start 
  digitalWrite(handlockB,  LOW);   // rel 13  closed on start
  digitalWrite(roofcolR ,  LOW);   // rel 14
  digitalWrite(blu24v ,    HIGH);  // rel 15  blue 24v light
  digitalWrite(red24v,     HIGH);   // rel 16  red 24v light

  for (int l=0; l < 6; l++) {
    pinMode(irled[l], OUTPUT);
    pinMode(charg[l],INPUT_PULLUP);
    digitalWrite(irled[l], LOW);
  }

  //delay(5000);

  digitalWrite(  vac_Out, LOW);    // BLK
  digitalWrite(  stopped, LOW);   // BRW 
  delay(100);
  digitalWrite(  seatAfd, LOW);   // WHT folded
  delay(100);
  digitalWrite(  seatBfd, LOW);   // ORA folded
  delay(100);
  digitalWrite(   bluTop, HIGH);    // YEL 12v game light  = ON ?
  delay(100);
  digitalWrite(   redTop, LOW);   // GRN 12v game light   =ON ?
  delay(100);
  digitalWrite(   toCharA, LOW);   // 
  delay(100);
  digitalWrite(   toCharB, LOW);   // 



drawBlue();
//lightBlue();

checkInputs();
Serial.println(" Everything ready, the players can be fixed and door should be closed..." );  

leerRemote();

inicio = millis();   // the start moment

digitalWrite(fans,       HIGH);  // rel 9  fans  HIGH = ON ???
    
delay(500);

Serial.println("#S|MAIN_01|[]#");  //  lets start the show !

delay(500); // for the display
dispA1();   // start the upper screen
dispB1();   // start the lower screen


timer.setInterval(1000,  showTime);

}
// eof_SETUP

void showTime() {
 //  Serial.println("Time is:");        
}

 
