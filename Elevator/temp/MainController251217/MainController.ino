//MainController 1.0.0
// level descriptions added   24/dec/2017
// Add win code
// Add loose code
// Add other actions between levels
// victory added and loose too

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
#define  operOut A12 // SIGNAL THE LEVEL ON OPERATOR DISPLAY + time left  
#define seatA A13  // dt-bps 2200 - BLU    HA
#define seatB A14  // dt-bps 2200 - WHT

/// ---DIRECT INPUTS ----
#define emerA     15 // ORA
#define emerB     14 // GRN
#define StopHU    52 // BK
#define roof      3  // BRW - GRN - GRN

////-- thru Addon OptoCoupler
#define REMOTE A0 // WHT  
#define tubes A15 // BRW
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
//t irled[6]  = { 26,  28,  30,  24,  32,  34 };
//t charg[6]  = { 48,  46,  44,  42,  40,  38 };  // hall sensors
///  ---IR LEDS ----
// se muestra el ( 3,   5,   2,   4,   1,   6 )
//int hollSensorPin[5] = {22, 23, 24, 25, 26};
//int pneuRelay[4] = {50, 51, 52, 53};
//int floorButts[4] = {46, 47, 48, 49};
//int extraExitPins[2] = {40, 42};
int irled[6]  = { 30,  32,  28,  24,  26,  34 };
int charg[6]  = { 44,  40,  46,  42,  48,  38 };  // hall sensors
int peopleCAddress = 20;

////----- variables ------
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
boolean skipLevel = false;
boolean darkScreen = false;
boolean deFibroClosed = true;
boolean upGameDone = false;
boolean gameLoose = false;
boolean showClock = true;
boolean gameWin = false;
boolean warnPlayed = false;
boolean fisrtSeatReady = false;
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
//=============== OBJECTS ===================
unsigned long startCount = 0;
void setup()
{
  Serial.begin(9600);
  Serial.println("#S|MAIN1|[]#");   //  lets start the show !
  Wire.begin();

  mp3_set_serial(Serial);
  mp3_set_volume(20);
  delay(50);

  LEDS.addLeds<WS2812, A1, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness( 20 );
  //  apagarTodo();

  pinMode (REMOTE    , INPUT_PULLUP);
  pinMode (tubes     , INPUT_PULLUP);
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
  pinMode (seatA     , INPUT_PULLUP);
  pinMode (seatB     , INPUT_PULLUP);

  pinMode(segmentClock,  OUTPUT);
  pinMode(segmentData ,  OUTPUT);
  pinMode(segmentLatch,  OUTPUT);

  pinMode(toLeak,        OUTPUT);

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
  pinMode(bluTop, OUTPUT);   // YEL
  pinMode(redTop, OUTPUT);   // GRN
  pinMode(toCharA, OUTPUT);   //
  pinMode(toCharB, OUTPUT);   //
  //  pinMode(  , OUTPUT);   //

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
  digitalWrite(HIDE1,      HIGH); // rel 1 hide 3 HIGH = CLOSED AND NO LIGHT
  digitalWrite(HIDE2,      HIGH); // rel 2 hide 4
  digitalWrite(HIDE3,      HIGH); // rel 3 hide 1
  digitalWrite(HIDE4,      HIGH); // rel 4 hide 2
  digitalWrite(HIDEMAS,    HIGH); // rel 5 hide +

  for (int l = 0; l < 6; l++) {
    pinMode(irled[l], OUTPUT);
    pinMode(charg[l], INPUT_PULLUP);
    digitalWrite(irled[l], LOW);
  }
  digitalWrite(DOOR,       LOW);  // OPEN main door

  digitalWrite(   vac_Out, LOW);  //no signal to VAC game
  digitalWrite(   toCharA, LOW);  //no signal to CHARGE 1
  digitalWrite(   toCharB, LOW);  //no signal to CHARGE 2
  digitalWrite(    toLeak, LOW);  //no signal to LEAKS

  digitalWrite(LeakMas,    HIGH); // leaks master valve = CLOSED
  digitalWrite(emerCall ,  HIGH); // emergency Call = OFF

  digitalWrite(  seatAfd,  LOW);   // turn off seat A folded LED
  digitalWrite(  seatBfd,  LOW);   // turn off seat B folded LED
  digitalWrite(  stopped,  LOW);   // turn off stopped LED

  digitalWrite(handlockA,  LOW);  // closed on start
  digitalWrite(handlockB,  LOW);  // closed on start


  digitalWrite(airSSR,     LOW); // shut off
  digitalWrite(airLOW,     HIGH); // shut off

  digitalWrite(sw12v  ,    HIGH); // rel 8 sw lights HIGH = ON
  digitalWrite(HDlight,    HIGH); // rel 6 HIGH = ON, Head Fans and lights
  digitalWrite(FANS,       HIGH);  // rel 9  fans  HIGH = ON
  digitalWrite(FOCUS,      HIGH);  // rel 10 LOW = ON, small light
  digitalWrite(blu24v ,    HIGH);  // OFF rel 15  blue 24v light
  digitalWrite(red24v,     HIGH);  // OFF  rel 16  red 24v light
  digitalWrite(bluTop,     HIGH);  // ON  roof light blu = ON
  digitalWrite(redTop,     LOW);   // OFF roof light red = OFF

  showNumber(60);

  drawBlue();

  FastLED.show();
  //lightBlue();
  //  checkInputs();
  Serial.println(" Ready " ); // the players can be fixed and door should be closed..." );
  leerRemote();
  inicio = millis();   // the start moment
  drawBlack();
  // digitalWrite(FANS,    HIGH);  // rel 9  fans  HIGH = ON this will short the power source B (!!!)
  //  digitalWrite(blu24v ,    LOW );  // rel 15  blue 24v light
  delay(500);
  delay(500); // for the display
  dispA1();   // start the upper screen

  //dispB1();   // start the lower screen
  //timer.setInterval(1000,  showTime);
  //delay(2000);
  //digitalWrite(DOOR, HIGH);  // closed main door

  Serial.println("#S|MAIN2|[]#");
  delay(500);
  sendToSlave(peopleCAddress, 0xFF);
  startCount = millis();
}

void loop()
{
  printCount();

  if (level == 10) // if stop handle is moved before seats are folded
  {
    if (digitalRead(StopHU) == LOW && warnPlayed == false) {
      warnPlayed = true;
      Serial.println("#S|FOLDSEATS|[]#");
    }
  }

  if (level == 0)
  {
    byte bufercnt = Wire.requestFrom(peopleCAddress, 1, false); //Request from operator params
    boolean p1State = false;
    boolean p2State = false;
    if (bufercnt == 1)
    {
      byte playerStates = Wire.read();
      p1State = (playerStates >> 2) & 0x01;
      p2State = (playerStates >> 6) & 0x01;

      //  Serial.println("player1State = " + String(p1State));
      //  Serial.println("player2State = " + String(p2State));

      if (p1State ) {
        digitalWrite (handlockA, LOW);
        if ( fisrtSeatReady == false ) {
          fisrtSeatReady = true;
          Serial.println("#S|UNLOCK1|[]#");
        }
      }

      if (p2State) {
        digitalWrite (handlockB, LOW);
        if ( fisrtSeatReady == false ) {
          fisrtSeatReady = true;
          Serial.println("#S|UNLOCK1|[]#");
        }
      }

    }

    if ((p1State && p2State) || skipLevel)
    {
      Serial.println("#S|UNLOCK2|[]#");
      // digitalWrite( handlockA, HIGH);
      delay(300);
      // digitalWrite( handlockB, HIGH);
      delay(300);
      Serial.println("Both players are free");
      Serial.println("waiting for seats to fold...");

      // dispA1();   // start the upper screen
      dispB1();   // start the lower screen
      level = 10;
      // digitalWrite(FANS,LOW);  // rel 9  fans  HIGH = ON
      if (skipLevel) {
        skipLevel = false;
        delay(300);
      }
    }
  }
  else if (level == 10) // Wait for both seats are folded.
  {
    boolean seatAFolded = !digitalRead(seatA);
    boolean seatBFolded = !digitalRead(seatB);
    if (seatAFolded) digitalWrite(seatAfd, HIGH);
    else digitalWrite(seatAfd, LOW);
    if (seatBFolded) digitalWrite(seatBfd, HIGH);
    else digitalWrite(seatBfd, LOW);

    if ((seatAFolded && seatBFolded) || skipLevel)
    {
      digitalWrite(seatAfd, HIGH);
      digitalWrite(seatBfd, HIGH);
      //Serial.println("#S|BOTHFOLDED|[]#");

      //mp3_play(1);
      Serial.println("Both seats are folded");
      Serial.println("Wait for STOP HANDLE...");
      //dispA1();   // start the upper screen
      dispB1();   // start the lower screen
      level = 20;
      if (skipLevel) {
        skipLevel = false;
        delay(300);
      }
    }
  }
  else if (level == 20) // Ждем остановки рычагом
  {
    boolean stopHandle = !readButt(HIGH, StopHU);
    if (skipLevel || stopHandle)
    {
      digitalWrite(stopped, HIGH);
      Serial.println("Video leaks");
      Serial.println("#S|MAIN3|[]#");
      dispA1();   // start the upper screen
      dispB1();   // start the lower screen
      Serial.println("capsule stopped, heavy shake and damage to the hull");
      Serial.println("LEAKS start");
      Goleaks();
      digitalWrite(HIDE1, LOW); // rel 1 hide 3 HIGH = CLOSED AND NO LIGHT
      Serial.println("release HIDE1 (solder)");
      //Останавливаем кабину (пневмо-команды)
      level = 30;
      digitalWrite(stopped, HIGH);
      if (skipLevel) {
        skipLevel = false;
        delay(300);
      }
    }
  }
  else if (level == 30) //Wait for leaks ok and VAC game started//
  {
    //    boolean leakOk = !readButt(HIGH, leakSig);
    //    if (leakOk || skipLevel)
    if (digitalRead(leakSig) == LOW || skipLevel)
    {
      Serial.println("#S|MAIN4|[]#");
      dispA1();   // start the upper screen
      dispB1();   // start the lower screen
      Serial.println("LEAKS game finished, signal to turn off the LEAKS");
      digitalWrite(toLeak, HIGH);
      delay(500);
      Serial.println("Master LEAKS valve closed");
      digitalWrite(LeakMas, HIGH);  // rel 11 HIGH - CLOSED - OFF
      delay(500);
      Serial.println(" Start VAC GAME ... " );
      digitalWrite(vac_Out, HIGH);
      delay(500);
      Serial.println(" ... free HIDE2 radio, time = time-15min " );
      digitalWrite(HIDE2, LOW);
      elapsedTime -= 15;

      Serial.println(" playing VAC game... " );
      Serial.println("    " );

      if (skipLevel)
      {
        Serial.println("Passed by operator");
        skipLevel = false;
        delay(300);
      }
      else Serial.println("Wonned by gamer");
      level = 40;

    }
  }
  else if (level == 40)
  {
    if (!readButt(HIGH, VacSig) || skipLevel)
    {
      if (skipLevel) {
        skipLevel = false;
        delay(300);
      }
      //   digitalWrite(vac_Out, LOW);    // forced finish for VAC game
      Serial.println("#S|ANTENNA|[]#");
      level = 50;
      dispA1();   // start the upper screen
      dispB1();   // start the lower screen
      Serial.println("VAC game done, coordinates sent");
      Serial.println("video on A (greetings from team)");
      Serial.println("release HIDE3 (extension) players should free the cpasule from lines");
      digitalWrite(HIDE3, LOW);
      Serial.println("********************************************************************");
      delay(500);
    }
  }
  else if (level == 50) //Roof level
  {
    if (!readButt(HIGH, roof) || skipLevel)
    {
      Serial.println("#S|DETACHED|[]#");
      delay(200);
      dispA1();   // start the upper screen
      dispB1();   // start the lower screen
      Serial.println("Capsule unhooked from lines");
      Serial.println("starting to go up");
      Serial.println("Octopus attack");
      Serial.println("Heavy shake and red lights");
      Serial.println("activate charge game");
      Serial.println("#S|MAIN5|[]#"); // Octopus Video???
      level = 60;
      sendToSlave(peopleCAddress, 0x02); // HALL Game activate on PeopleController
      delay(500);
      if (skipLevel) {
        skipLevel = false;
        delay(300);
      }
      //
      // lights
      // damages to the electric system
      // red lights
    }
  }
  else if (level == 60) // Octopus attack
  {
    if (!readButt(HIGH, CharSigA) || skipLevel)
    {
      Serial.println("#S|MAIN6|[]#");
      Serial.println("************************************************************");
      Serial.println("all energy transfered to external armor");
      Serial.println("lights strikes");
      Serial.println("and blackout...");
      Serial.println("half of CHARGE game done, turn lights off (all display dark)");
      dispA1();   // start the upper screen
      dispB1();   // start the lower screen
      Serial.println("#S|POWERSURGE|[]#");
      digitalWrite(sw12v  ,    LOW); //OFF
      digitalWrite(HDlight,    LOW); //OFF
      digitalWrite(FANS,       LOW); //OFF
      digitalWrite(FOCUS,      HIGH);//OFF
      digitalWrite(blu24v ,    HIGH);//OFF
      digitalWrite(red24v,     HIGH);//OFF
      digitalWrite(bluTop,     LOW); //OFF
      digitalWrite(redTop,     LOW); //OFF
      Serial.println("release HIDE4 (IR charger)");
      digitalWrite(HIDE4, LOW);
      Serial.println("expect second signal from CHARGE") ;
      Serial.println("********************************************** ");
      level = 70;
      sendToSlave(peopleCAddress, 0x03); // Send to processing dark screen mode
      if (skipLevel)
      {
        chargePower();
        skipLevel = false;
        delay(500);
      }
    }
    //  wait for tubes >> open HIDE - Defibrilator !!!skip first hidemas second charge
    if (deFibroClosed)
    {
      byte bufercnt = Wire.requestFrom(peopleCAddress, 1, false);
      if (bufercnt == 1)
      {
        byte hallDone = Wire.read();
        if (hallDone == 0x01)
        {
          Serial.println("Defibro opened");
          digitalWrite(HIDEMAS, LOW);
          deFibroClosed = false;
        }
      }
    }
  }
  else if (level == 70)
  {
    if (!readButt(HIGH, CharSigA) || skipLevel)
    {
      Serial.println("energy recharged, ");
      Serial.println("CHARGE game done, turn lights on");
      Serial.println("#S|MAIN7|[]#");
      dispA1();   // start the upper screen
      dispB1();   // start the lower screen
      digitalWrite(sw12v  ,    HIGH);  // HIGH = ON
      digitalWrite(HDlight,    HIGH);  // HIGH = ON, Head Fans and lights
      digitalWrite(FANS,       HIGH);  // HIGH = ON
      digitalWrite(FOCUS,      LOW);   // LOW = ON, small light
      digitalWrite(blu24v ,    LOW);   // ON
      digitalWrite(red24v,     HIGH);  // OFF
      digitalWrite(bluTop,     HIGH);  // ON
      digitalWrite(redTop,     LOW);   // OFF
      Serial.println("Start video ");
      Serial.println(" start the GoUp game");
      sendToSlave(peopleCAddress, 0x04); //Send to processing start upGame.
      level = 80;
      Serial.println("#S|DANCE|[]#");
      if (skipLevel)
      {
        chargePower();
        skipLevel = false;
        delay(500);
      }
    }
  }
  else if (level == 80) //upGame wait
  {
    byte bufercnt = Wire.requestFrom(peopleCAddress, 1, false);
    if (bufercnt == 1)
    {
      byte upGameDone = Wire.read();
      if (upGameDone == 0x01 || skipLevel)
      {
        level = 100;
        gameWin = true;
        skipLevel = false;
        //Video, Sound   ******************************************************************* VICTORY !!!!!!!!!!!!!
        sendToSlave(peopleCAddress, 0x05); //Send to people and to processing win splash start.
      }
    }
  }
  else if (level == 100) //Wait for player going out. Win or Loose.
  {
    //Win Game, restart...
  }
  remoteState[0] = readButt(remoteState[1], REMOTE);
  if (!remoteState[0] && remoteState[1])
  {
    if (level == 60 && deFibroClosed)
    {
      deFibroClosed = false;
      digitalWrite(HIDEMAS, LOW);
    }
    else skipLevel = true;
    Serial.println("*****************************");
    Serial.println("** Operator skip / Level = " + String(level));
    Serial.println("*****************************");
  }
  remoteState[1] = remoteState[0];
}

boolean readButt(boolean prevState, int pin)
{
  boolean currState = digitalRead(pin);
  if (currState != prevState)
  {
    delay(5);
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
  //leaks = true;
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
  Serial.print("tubes    = ");
  Serial.println(digitalRead(tubes));   //     = A15; // BRW
  Serial.print("Emer A   = ");
  Serial.println(digitalRead(emerA));   //     = 15; // ORA
  Serial.print("Emer B   = ");
  Serial.println(digitalRead(emerB));   //     = 14; // GRN
  Serial.print("CharSigA = ");
  Serial.println(digitalRead(CharSigA));//   = 4;  // RD
  Serial.print("CharSigB = ");
  Serial.println(digitalRead(CharSigB)); //   =  7; // GRN
  Serial.print("StopHU   = ");
  Serial.println(digitalRead(StopHU));  //   = 52; // BK
  Serial.print("VacSig   = ");
  Serial.println(digitalRead(VacSig));  //   = 5;  // BK
  Serial.print("LeakSig  = ");
  Serial.println(digitalRead(leakSig));//    = 6;  // WHT
  Serial.print("seatA    = ");
  Serial.println(digitalRead(seatA));  //      = A13;
  Serial.print("seatB    = ");
  Serial.println(digitalRead(seatB));  //      = A14;
  Serial.print("roof     = ");
  Serial.println(digitalRead(roof));   //       = 3;
  Serial.print("Level    = ");
  Serial.println(level);
}

void leerRemote()
{
  Serial.println(" press Remote/Start/skip to continue...");
  while (digitalRead(REMOTE)) {
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
  if (tick % 1000 == 0)
  {
    if (gameLoose) showClock = !showClock;
    if (showClock) showNumber(elapsedTime);
    else showNumber(' ');
  }
  if (tick - startCount >= 60000 && !gameLoose && !gameWin)
  {
    Serial.println("COUNT=" + String(tick / 60000));
    if (elapsedTime == 0)
    {
      //YOU LOOSE, START FLASH CLOCKS *********************************************************************************
      // Add some actions of loose this
      // Message of Loose.
      level = 100;
      gameLoose = true;
    }
    else elapsedTime--;
    startCount = tick;
  }
}


void chargePower()
{
  digitalWrite(toCharA, HIGH);   // activate Charger
  delay(200);
  digitalWrite(toCharA, LOW);
}


