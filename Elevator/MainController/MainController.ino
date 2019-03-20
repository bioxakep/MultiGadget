#include <LedControl.h>

// MainController 1.1.0
// level descriptions added   24/dec/2017
// Add win code
// Add loose code
// Add other actions between levels
// victory added and loose too
// 26/dec/CHARGING modeule added, effects adjusted, CHARGE linked,
// 27/DEC/2017 missing pneumatic added - not finished
// 28/dec/2017 timing adjustments
// 31/dec/2017 video and audio tracks added and revised
// 02/JAN/2018 pneumatics added
// 05/JAN/2018 charging game updated, ir bllinks added   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// 08.01.2018 clock change, add readButt to all digiread
// 10/JAN/2018 Admin module signal added
// 11/jan/2018 shaking module added
// 17/JAN/2018 SHAKING ADJUSTED

//============= LIBRARIES ATTACH ===========
#include "Wire.h" // I2C
#include "SoftwareSerial.h" // for RFID serial connection
#include "FastLED.h"
#include <DFPlayer_Mini_Mp3.h>
//============= LIBRARIES ATTACH ===========
//============= PIN & ADDRESS DEFINE =================
#define NUM_LEDS 160
#define DATA_PIN A1

#define HIDE1      37  // rel 1
#define HIDE2      35  // rel 2
#define HIDE3      33  // rel 3
#define HIDE4      31  // rel 4
#define HIDEMAS    29  // rel 5
#define HDlight    27  // rel 6
#define DOOR       25  // rel 7
#define sw12v      23  // rel 8
#define FANS       39  // rel 9
#define FOCUS      41  // rel 10
#define LeakMas    43  // rel 11
#define handlockA  45  // rel 12
#define handlockB  47  // rel 13
#define roofcolR   49  // rel 14 // unused - free
#define blu24v     22  // rel 15 
#define red24v     A9  // rel 16 
#define emerCall   49  // rel 14 // send a short signa to operator then any emergency button is pressed

// ---- VIDEO PLAY ONE ------  UPPER SCR   ---  LEFT PLAYER
#define videoA1 12  // GRN - BRW
#define videoA2 11  // WHT - ORA
#define videoA3 10  // RED - GRN

// ---- VIDEO PLAY TWO ------ LOWER SCR    --- RIGHT PLAYER
#define videoB1 16  // GRN - BRW
#define videoB2 17  // WHT - ORA
#define videoB3 18  // WHT - XXX  ???
#define videoB4 19  // WHT - XXX  ???

///-----TRANSISTORS-----
#define  vac_Out A2  // BLK
#define  stopped A3  // BRW
#define  seatAfd A4  // WHT
#define  seatBfd A5  // ORA 
#define   bluTop A6  // YEL game
#define   redTop A7  // GRN game
#define  toCharA A10 // BRW  -12v
#define  toCharB A11 // WT   -12v
#define  operOut A12 // SIGNAL THE LEVEL ON OPERATOR DISPLAY + time left  short to next / long to reset
#define    seatA A13 // dt-bps 2200 - BLU    HA
#define    seatB A14 // dt-bps 2200 - WHT

/// ---DIRECT INPUTS ----
#define emerA     15 // ORA
#define emerB     14 // GRN
#define StopHU    52 // BK
#define roof      3  // BRW - GRN - GRN

////-- thru Addon OptoCoupler
#define REMOTE A0 // WHT  
//#define tubes A15 // BRW
/// ---- SPARE OPTO RES-----
#define spareOUT_optoRES1 8
#define toLeak    9 // optoresistor GRN/RED

///  --- OPTO INPUTS ----
#define CharSigA  7  // GRN
#define VacSig    5  // BK
#define leakSig   6  // WHT-GRN - ORA/thn
#define CharSigB  4  // RD 
#define CharSigC  A15 // BRW - OPTO - BLU - Y - wh   signal from CHARGER GAME = fan control
// --- extra output ----
#define airSSR    36  // BK  110v
#define airLOW     2  // WH  110v
/// --- CLOCK ----
#define segmentClock 51 // RED - BRW  
#define segmentLatch 53 // WHT - GRY
#define segmentData  50 // GRN - BLU 

///  --- IR LEDS ORIGINAL/PHYSICAL ----
// se muestra el ( 3,   5,   2,   4,   1,   6 )
bool irON[6]  = {  1,   1,   1,   1,   1,   1 };  //status of ir leds  
int irled[6]  = { 30,  32,  28,  24,  26,  34 };
int charg[6]  = { 44,  40,  46,  42,  48,  38 };  // hall sensors
int peopleCAddress = 20;

////----- variables ------
int  LED       = 13;
int  number    = 60;
int  level     = 0;
int  i         = 0;
int  lastlevel = 0;
long inicio    = 0;
byte elapsedTime = 60;
byte startTimeAdding = 15;
boolean chargingGame = false;
boolean tubesPass    = false; // pressure check on right hose
boolean chargeGame   = false;
boolean remoteState[2]  = {HIGH, HIGH};
boolean huStates[2] = {HIGH, HIGH};
boolean vacStates[2] = {HIGH, HIGH};
boolean roofStates[2] = {HIGH, HIGH};
boolean CharSigAStates[2] = {HIGH, HIGH};
boolean skipLevel = false;
boolean darkScreen = false;
boolean deFibroClosed = true;
boolean upGameDone = false;
boolean gameLoose = false;
boolean showClock = true;
boolean gameWin = false;
boolean warnPlayed = false;
boolean fisrtSeatReady = false;
boolean FANstate  = true;
boolean STAstate  = true; //stabilization state - controlled by CHARGE
bool soundOK = true;
long start2shake = 0;
long isTime2shake = 0;
bool moved = false;
long nextShake = 50000;
long shakeMoment = 0;
long showTime = 0;
long irtime = 0;
int Admin   = 0;
int Move = 0;
////  ------var for charging game --------------
int bar          = 0;
int l            = 0;
int actual [6]   = {  3,  5,  2,  4,   1,   6 };
int chBlock[6]   = { 26, 51, 66, 92, 132, 160 };
int chCharged[6] = {  0,  0,  0,  0,   0,   0 };
int actBlock     = 0;
//=============== STATES ===================
//=============== OBJECTS ===================
/* For Slaves
   mySwitch.setPulseLength(405); 410
   mySwitch.setProtocol(1); 2
*/
CRGB leds[NUM_LEDS];
LedControl clocks=LedControl(segmentData,segmentClock,segmentLatch,1);
//=============== OBJECTS ===================
unsigned long startCount = 0;
void setup()
{
  Serial.begin(9600);
  Serial.println("#S|MAIN1|[]#");   //  lets start the show !
  Wire.begin();

  mp3_set_serial(Serial);
  mp3_set_volume(30);
  delay(30);

  LEDS.addLeds<WS2812, A1, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness( 60 );
  //  apagarTodo();

  pinMode (REMOTE    , INPUT_PULLUP);
 // pinMode (tubes     , INPUT_PULLUP);
  pinMode (CharSigA  , INPUT_PULLUP);
  pinMode (VacSig    , INPUT_PULLUP);
  pinMode (leakSig   , INPUT_PULLUP);
  pinMode (CharSigB  , INPUT_PULLUP);
  pinMode (CharSigC  , INPUT_PULLUP);
  pinMode (roof      , INPUT_PULLUP);

  pinMode (StopHU    , INPUT_PULLUP);

  pinMode (emerA     , INPUT_PULLUP);
  pinMode (emerB     , INPUT_PULLUP);
  pinMode (emerCall  , OUTPUT);
  pinMode (operOut   , OUTPUT);
  pinMode (seatA     , INPUT_PULLUP);
  pinMode (seatB     , INPUT_PULLUP);

  pinMode(segmentClock,  OUTPUT);
  pinMode(segmentData ,  OUTPUT);
  pinMode(segmentLatch,  OUTPUT);

  pinMode(toLeak,        OUTPUT);

  pinMode (LED       ,   OUTPUT);

  pinMode(HIDE1,     OUTPUT);  // rel 1
  pinMode(HIDE2,     OUTPUT);  // rel 2
  pinMode(HIDE3,     OUTPUT);  // rel 3
  pinMode(HIDE4,     OUTPUT);  // rel 4
  pinMode(HIDEMAS,   OUTPUT);  // rel 5
  pinMode(HDlight,   OUTPUT);  // rel 6
  pinMode(DOOR,      OUTPUT);  // rel 7
  pinMode(sw12v  ,   OUTPUT);  // rel 8

  pinMode(FANS,      OUTPUT);  // rel 9
  pinMode(FOCUS,     OUTPUT);  // rel 10
  pinMode(LeakMas,   OUTPUT);  // rel 11
  pinMode(handlockA, OUTPUT);  // rel 12
  pinMode(handlockB, OUTPUT);  // rel 13
  pinMode(roofcolR , OUTPUT);  // rel 14
  pinMode(blu24v ,   OUTPUT);  // rel 15
  pinMode(red24v,    OUTPUT);  // rel 16

  pinMode(vac_Out, OUTPUT);   // BLK
  pinMode(stopped, OUTPUT);   // BRW
  pinMode(seatAfd, OUTPUT);   // WHT
  pinMode(seatBfd, OUTPUT);   // ORA
  pinMode(bluTop, OUTPUT);    // YEL
  pinMode(redTop, OUTPUT);    // GRN
  pinMode(toCharA, OUTPUT);   //
  pinMode(toCharB, OUTPUT);   //
  //  pinMode(  , OUTPUT);    //

  pinMode(airSSR, OUTPUT);   // BK
  pinMode(airLOW, OUTPUT);   // BK

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

  // ------ LOCKS --------
  digitalWrite(HIDE1,      LOW); // rel 1 hide 3 HIGH = CLOSED AND NO LIGHT
  digitalWrite(HIDE2,      LOW); // rel 2 hide 4
  digitalWrite(HIDE3,      LOW); // rel 3 hide 1
  digitalWrite(HIDE4,      LOW); // rel 4 hide 2
  digitalWrite(HIDEMAS,   HIGH); // rel 5 hide +

  for (int l = 0; l < 6; l++) {
    pinMode(irled[l], OUTPUT);
    pinMode(charg[l], INPUT_PULLUP);
    digitalWrite(irled[l], LOW);
  }


  digitalWrite(   operOut, HIGH);  //start long signal to reset the AdminModule
  digitalWrite(   vac_Out, LOW);  //no signal to VAC game
  digitalWrite(   toCharA, LOW);  //no signal to CHARGE 1
  digitalWrite(   toCharB, LOW);  //no signal to CHARGE 2
  digitalWrite(    toLeak, LOW);  //no signal to LEAKS

  digitalWrite(       LED, LOW);  //no signal to LEAKS

  digitalWrite(LeakMas,    HIGH); // leaks master valve = CLOSED
  delay(300);
  digitalWrite(emerCall ,  HIGH); // emergency Call = OFF
  delay(300);
  mp3_play(1);
  delay(300);
  digitalWrite(  seatAfd,  LOW);   // turn off seat A folded LED
  delay(300);
  digitalWrite(  seatBfd,  LOW);   // turn off seat B folded LED
  delay(300);
  digitalWrite(  stopped,  LOW);   // turn off stopped LED
  delay(300);
  digitalWrite(handlockA,  LOW);  // closed on start
  delay(300);
  digitalWrite(handlockB,  LOW);  // closed on start
  delay(300);


  digitalWrite(airSSR,     LOW); // FULL DOWN
  delay(300);
  digitalWrite(airLOW,     LOW); // FULL DOWN
  delay(300);

  digitalWrite(sw12v  ,    HIGH); // rel 8 sw lights HIGH = ON
  delay(300);
  digitalWrite(HDlight,    LOW); // rel 6 HIGH = ON, Head Fans and lights  // UNPLUGGED  possible short
  delay(300);
  digitalWrite(FANS,       HIGH);  // rel 9  fans  HIGH = ON
  delay(300);
  digitalWrite(FOCUS,      LOW);  // rel 10 LOW = ON, small light
  delay(300);
  digitalWrite(blu24v ,    HIGH);  // OFF rel 15  blue 24v light
  delay(300);
  digitalWrite(red24v,     HIGH);  // OFF  rel 16  red 24v light
  delay(300);
  digitalWrite(bluTop,     HIGH);  // ON  roof light blu = ON
  delay(300);
  digitalWrite(redTop,     LOW);   // OFF roof light red = OFF
  delay(300);

  digitalWrite(DOOR, LOW); // OPEN      LOW);  // CLOSE main door
  delay(300);
 // digitalWrite(   operOut, LOW);  //stop long signal to reset the AdminModule - should show 0_00 sfter receiving it
 
  Serial.println("  ");
  Serial.println(" RESET signal sent to Amdin");
  showNumber(96);

  // drawBlue();
  drawBlack();

  FastLED.show();
  // lightBlue();
  // checkInputs();
  // Serial.println(" Ready " ); // the players can be fixed and door should be closed..." );
  Serial.println(" ");
  Serial.println("#S|TRACK1|[]#"); // audio track
  delay(5000);
  digitalWrite(   operOut, HIGH);  //move AdminModule one level up
  delay(100);


  leerRemote();   //// *****************************   START THE GAME ****************************************************

      Admin++;
 //     Serial.print(" Signal to Admin sent...");
 //     Serial.println(Admin);
      digitalWrite(   operOut, LOW);   

  drawBlue();
 
  Serial.println("#S|TRACK2|[]#"); // audio track
  digitalWrite(HIDE1,      HIGH); // rel 1 hide 3 HIGH = CLOSED AND NO LIGHT
  delay(200);
  digitalWrite(HIDE2,      HIGH); // rel 2 hide 4
  delay(200);
  digitalWrite(HIDE3,      HIGH); // rel 3 hide 1
  delay(200);
  digitalWrite(HIDE4,      HIGH); // rel 4 hide 2
  delay(200);
  digitalWrite(HIDEMAS,    HIGH); // rel 5 hide +
  delay(200);
  digitalWrite(DOOR, HIGH);       // CLOSED
  delay(300);

  inicio = millis();   // the start moment
  pnUP(3000);
  delay(300);
  digitalWrite(FANS, LOW); // spin the roof fans
  delay(300);
  level = 0;
  dispA1();   // start the upper screen , the WELCOME message
  delay(32000); /// delay for the long of the message
  Serial.println("#S|MAIN2|[]#");
  delay(500);
  sendToSlave(peopleCAddress, 0xFF);
  startCount = millis();
  clocks.shutdown(0,false);
  /* Set the brightness to a medium values */
  clocks.setIntensity(0,8);
  /* and clear the display */
  clocks.clearDisplay(0);
}
//eof_setup

boolean readButt(boolean prevState, int pin)
{
  boolean currState = digitalRead(pin);
  if (currState != prevState)
  {
    delay(10);
    currState = digitalRead(pin);
  }
  return currState;
}

void Goleaks() {
  digitalWrite(toLeak, HIGH);
  delay(200);
  digitalWrite(toLeak, LOW);
  delay(200);
  digitalWrite(LeakMas, LOW);  // rel 11 OPEN - ON
  delay(500);
}

void checkInputs() {

  Serial.println(">");

  for (int k = 0; k < 6; k++) {
    Serial.print(digitalRead(charg[k]));
    Serial.print("-");
  }
  Serial.println(" ");
  Serial.print("Remote   = ");
  Serial.println(digitalRead(REMOTE));  //    = A0; // WHT
  Serial.print("Emer A   = ");
  Serial.println(digitalRead(emerA));   //     = 15; // ORA
  Serial.print("Emer B   = ");
  Serial.println(digitalRead(emerB));   //     = 14; // GRN
  Serial.print("CharSigA = ");
  Serial.println(digitalRead(CharSigA));//     = 4;  // RD
  Serial.print("CharSigB = ");
  Serial.println(digitalRead(CharSigB)); //    =  7; // GRN
  Serial.print("StopHU   = ");
  Serial.println(digitalRead(StopHU));  //     = 52; // BK
  Serial.print("VacSig   = ");
  Serial.println(digitalRead(VacSig));  //     = 5;  // BK
  Serial.print("LeakSig  = ");
  Serial.println(digitalRead(leakSig));//      = 6;  // WHT
  Serial.print("seatA    = ");
  Serial.println(digitalRead(seatA));  //      = A13;
  Serial.print("seatB    = ");
  Serial.println(digitalRead(seatB));  //      = A14;
  Serial.print("roof     = ");
  Serial.println(digitalRead(roof));   //      = 3;
  Serial.print("Level    = ");
  Serial.println(level);
}

void leerRemote()
{
  Serial.println(" skip to continue...");
  while (digitalRead(REMOTE))  {
    ;
  }
  delay(10);
  while (!digitalRead(REMOTE)) {
    ;
  }
  delay(100);
}

void sendToSlave(int address, byte data)
{
  Wire.beginTransmission(address); // transmit to device #8
  Wire.write(data);
  Wire.endTransmission();
}

void printCount()
{
  unsigned long tick = millis();
  if(tick - showTime >= 1000 && !gameWin && !gameLoose)
  //if (tick - showTime >= 1000 && !gameWin)
  {
    String eliTimeStr = String(elapsedTime);
    if (gameLoose) showClock = !showClock;
    if (showClock) 
    {
     
   //   Serial.println("EliTime = "+String(elapsedTime));
      for(int i = 0; i < eliTimeStr.length(); i++) clocks.setChar(0,1-i,eliTimeStr[i],false);
      for(int i = eliTimeStr.length(); i < 2; i++) clocks.setChar(0,1-i,' ',false);
      //showNumber(elapsedTime);
    }
    else 
    {
      //showNumber(' ');
      for(int i = 0; i < 2; i++) clocks.setChar(0,i,' ',false);
    }
    //  Serial.println("SHOWCLOCK="+String(showClock));
    showTime = tick;
  }

if (tick - startCount >= 60000 && !gameWin && elapsedTime > 0)
  {
    //  Serial.println("COUNT=" + String(tick / 60000));
    elapsedTime--;
    if (elapsedTime == 0)
    {
      Serial.print("GAME LOOSE");
      digitalWrite(DOOR, LOW); //OPEN
      gameLoose = true;
    }
//    Serial.println("EliTime = " + String(elapsedTime));
    startCount = tick;
  }



}
  
  
  
 


void chargePower()
{
  digitalWrite(toCharA, HIGH);   // activate Charger
  delay(200);
  digitalWrite(toCharA, LOW);
}

void moveAdmin()
{
      digitalWrite(operOut, HIGH);  //move AdminModule one level up
      delay(100);
      Admin++;
     // Serial.print(" Signal to Admin sent...");
     // Serial.println(Admin);
      digitalWrite(operOut, LOW);
}
