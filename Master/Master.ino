// 30.OCT.2018  - 31.OCT.2018 - 11.DEC.2018
// Rewrote Connection
// 27.nov.2018 crystalRec removed (now located on light controller as bigKey)
// 28 .nov.2018 voicePin reversed 
// Master SKY  - Mega
// I2C master - linked to Motor_Controller & Lights_Controller & World
// RS-485 interface - Serial1 - link to Admin
// removed - xBee - Serial3 - link to light_controller and World_prop
// mp3_player - on Serial(TX)
#include "Wire.h" // I2C
#include <OneWire.h>
#include <ArdCPZ.h>
#include <DFPlayer_Mini_Mp3.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
//I2C PINS: 20-21
#define SSerialTxControl 17   //RS485 Direction control

LiquidCrystal_I2C lcd(0x3F, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

int windRFPin  = A0;    // RFID key to start
int underRFPin  = 9;    // RFID key to the underground door
int rainRFPin = 12;

int triPin = 11;

boolean fireState = false;
int firePin = 3;

//Итого:  , worldAdd (факел) = 3

int curHighPin = -1;

//address i2c
int lightConAddr = 20;
int motorConAddr = 21;

//Gadget states
boolean operGStates[32];
boolean passGStates[32];
byte voiceHintStates[32];
// lev 0
//
/*
  START (cmds 0x1x)
  Start 0x10 both, 0x11 light (no OP Control)
  0 Balloon 0x12
  1 Press none
  2 Gate 0x13

  LEVEL 90

  THUNDER (cmds 0x2x)
  3 Poseidon
  4 Trident 0x21 motor
  5 Demetra
  6 Rain 0x22 motor
  7 Vine
  8 Dionis-1
  9 Hercules
  10 Narcis
  11 Thunder

  SHIELDS (cmds 0x3x)
  12 Afina-1
  13 Afina-2
  14 Time
  15 Octopus
  16 Note
  17 Wind  0x31 both
  18 Ghera-1

  SEALS (cmds 0x4x)
  19 Fire 0x41 light turner
  20 Flower-1
  21 Flower-2 0x42 turner off
  22 Dionis-2
  23 Arpha
  24 Ghera-2

  UNDERGROUND (cmds 0x5x)
  25 Under (RFID wait) 0x51 motor
  26 Zodiak
  27 Minot
  28 Gorgona
  29 Crystals 0x55 light, 0x99 motor
  30 Light

  END (cmds 0x6x)
  31 WIN
*/
String gadgetNames[32] = {"Baloon    ", "Press    ", "Gate     ",
                          "Poseidon  ", "Trident  ", "Demetra-1", "Rain     ", "Vine     ", "Dionis-1 ", "Hercules ", "Narcis   ", "Thunder  ",
                          "Afina-1   ", "Afina-2  ", "Time     ", "Octopus  ", "Note     ", "Wind     ", "Ghera-1  ",
                          "Fire      ", "Flower-1 ", "Flower-2 ", "Arpha    ", "Dionis-2 ", "Ghera-2  ",
                          "BigKey    ", "Under    ", "Zodiak   ", "Minot    ", "Gorgona  ", "Cristals ",
                          "End       "
                         };
//Voice pin
int voicePin = 6;     // HIGH then pressed, normally LOW 
boolean voiceStates[2] = {HIGH, HIGH};
//START
int start = 0;
int startPin = 8;
boolean startStates[2] = {HIGH, HIGH};

//BALOON
byte baloon = 0;
int balloIN  = 50;   // a.
int balloOUT = 44;

//PRESS
byte presss = 1;
int pressIN  = 48;   // b.
int pressOUT = 42;

//GATE
ArdCPZ *cpz3; // Gate RFIDReader
byte gate = 2;
int gateRFPin   = 10;   // RFID key to gate

boolean gateRFWait = true;

//POSEIDON
byte poseidon = 3;
int poseiIN  = 53;   // h.
int poseiOUT = 43;
int poseiHD = 21; // CHOOSE PIN

//TRIDENT
byte trident = 4;
unsigned long tridentTimer = 0;
unsigned long tridentDelay = 5000;
boolean tridentWait = true;

//DEMETRA
byte demetra = 5;
int demetIN  = 26;   // d.
int demetOUT = 38;
int demetHD  = A1;
unsigned long demetTimer = 0;
unsigned long demetDelay = 6300;

//RAIN
byte rain = 6;
ArdCPZ *cpz4; //Rain
int rainBeacon  = 410;
boolean rainRFWait = true;

//VINE
byte vine = 7;
int vinemIN  = 24;   //e.
int vinemOUT = 36;

//DIONIS
byte dionis1 = 8;
byte dionis2 = 23;
int dioniIN  = 22;   //f.
int dioniOUT = 34;
int dioniHD1 = A5;   // piston
int dioniHD2 = A3;   // safe
unsigned long dioni1Timer = 0;
unsigned long dioni2Timer = 0;
unsigned long dioni2Delay = 5000;

//HERCULES
byte hercul  = 9;
int hercuIN  = 7;
int hercuHD  = A4;
unsigned long hercuTimer = 0;

//NARCIS
byte narcis  = 10;
int narciIN  = 51;   // i.  /// video seen
int narciOUT = 41;          /// launch video

//THUNDER
byte thunder = 11;
int thundIN  = 52;   // g.
int thundOUT = 32;

//AFINA
byte afina1 = 12;
byte afina2 = 13;
int afinaIN  = 27;   // m.
int afinaOUT = 35;   //
int afinaHD1 = A7;
unsigned long afinaTimer = 0;
unsigned long afinaHDdelay = 8000;
int afinaHD2 = A8;

//TIME
byte Time    = 14;
int timeOUT  = 38;
int timeIN   = 46;

//OCTOPUS
byte octopus = 15;
int octopIN  = 47;   // k.
int octopOUT = 37;

//NOTE
byte note = 16;
int noteIN   = 49;   // j.
int noteOUT  = 39;
int noteHD   = A6;
unsigned long noteTimer = 0;
unsigned long noteHDDelay = 3300;
//WIND
byte wind = 17;
boolean windRFWait = true;

//MUSES
int musesIN  = 23;   // o. to change their messages
int musesOUT = 31;   // correct pattern

//GHERA
byte ghera1 = 18;
byte ghera2 = 24;
int gheraIN   = 4;
int gheraOUT  = 2;

//FIRE
byte fire = 19;

//FLOWERS
byte flower1 = 20;
byte flower2 = 21;
int flowrIN  = 25;
int flowrOUT = 33;   //n.
int flowrHD  = A13;
unsigned long flowerTimer = 0;
unsigned long flowerDelay = 5550;

//ARPHA
byte arpha = 22;
int arphaHD  = A9;
unsigned long arphaTimer = 0;
unsigned long arphaDelay = 22000;

//BIGKEY         
byte bigkey = 25;
int bigKeyIN = 16;

//UNDERGROUND
byte under = 26;
ArdCPZ *cpz2; //Under
int underBeacon = 328;
boolean underRFWait = true;

//ZODIAK
byte zodiak = 27;
int zodiaIN  = 5;
int zodiaHD  = A11;

//MINOT
byte minot = 28;
int minotIN  = 29;   // B.
int minotOUT = 45;
int minotHD  = A10;

//GORGONA
byte gorgona = 29;
int gorgoIN  = 28;   // A.
int gorgoOUT = 30;
int gorgoHD  = A12;

//CRISTALS
byte crystals = 30;
boolean crystStates[3] = {false, false, false};
boolean crystDone = false;


byte win = 31;

int totalGadgets = 32;

int spare = A2;  ////////////////////  TIP  controlled spare OUTPUT

byte level = 10;

boolean shieldDone = false;
boolean thunderDone = false;
boolean sealsDone  = false;

boolean bridgeConnected = false;
unsigned long startHighPin = 0;
unsigned long lastA9SentTime = 0;
unsigned long lastRFIDCheck = 0;

unsigned long HDDelay = 3000;

void setup()
{
  Wire.begin();
  Wire.setClock(10000);

  Serial.begin (9600);
  Serial.println("I2C Started");
  delay(10);
  Serial1.begin(9600);  //RS-485 to Bridge
  Serial3.begin(9600);  //RS-485 to Bridge
  delay(10);
  Serial.println("SKY Master v2.1");
  Serial.println("30 OCT 2018   -  11 DEC - 2018");
  Serial.println("RS485 Started.");
  pinSetup(); //from pinWorker

  Serial.println("\nmain mp3 player test");
  delay(100);
  mp3_set_serial(Serial);
  delay(100);
  mp3_set_volume (28);
  delay(100);
  mp3_stop(); 
  delay(200);

  Serial.println("\nhint mp3 player test");
  mp3_set_serial(Serial3);
  delay(100);
  mp3_set_volume (28);
  delay(100);
  mp3_play(4); // test hint sound
  delay(200);

  Serial.println("\n--------------");

  cpz2 = new ArdCPZ(underRFPin);
  cpz3 = new ArdCPZ(gateRFPin);
  Serial.println("RFID Connected");

  for (int g = 0; g < totalGadgets; g++)
  {
    operGStates[g] = false;
    passGStates[g] = false;
    voiceHintStates[g] = 0;
  }


  delay(10);
  checkInputs();
  Serial.println("\nInputs Checked");
  delay(10);
  openLocks();
  Serial.println("\nLocks Opened");
  lcd.init();
  lcd.backlight();
  lcd.print("Initialising....");

  connectToBridge();

  lcd.clear();
  lcd.print("Connected");
  lcd.setCursor(0, 1);
  lcd.print("Wait start pin");
}

void loop()
{
  unsigned long tick = millis();

  startStates[0] = debounce(startStates[1], startPin); // READ START BUTTON
  voiceStates[0] = debounce(voiceStates[1], voicePin);
  if (level > 10 && !startStates[0] && startStates[1]) skipNextGadget(); // SKIP BY START BUTTON
  if (level > 10 && voiceStates[0] && !voiceStates[1]) voiceCurGadget();
  if (curHighPin > 0) // WATCH FOR ACTIVE PIN
  {
    if (tick - startHighPin > 250)
    {
      digitalWrite(curHighPin, LOW);
      curHighPin = -1;
    }
  }

  getOperSkips();
  if (level == 10) Start(); // Start button pressing 2 times wait
  else if (level == 12) Baloon(); //#0 if players never finish ballon, operator can skip it here (send signal to ballon)
  else if (level == 13) Press(); //#1 if players never finish press, operator can skip it here (send signal to press)
  else if (level == 14) Gate(); //#2 if players never find the key, operator can skip it here > gateOpen = true;
  else if (level == 90)
  { // on level 90 all events may be done in any order/sequence
    if (tick - lastRFIDCheck > 100)
    {
      lastRFIDCheck = tick;
      if (windRFWait) windRFWait = !getWindRFID();
      if (rainRFWait) rainRFWait = !getRainRFID();
    }
    
//    tick = millis();
    
    // ==================================== THUNDER ==================================
    if (!thunderDone)
    {
      Poseidon();//#3
      Trident();//#4
      if(tridentTimer > 0 && ((millis() - tridentTimer) > tridentDelay)) 
      {
       sendToSlave(motorConAddr, 0x21); // Column Down ++++ EARTHQUAKE !!!!!!!!!!!!!!!!!!!!!!
       Serial.println("Trident done "+ String(tick));
       tridentTimer = 0;
      }

      Demetra();//#5
      if(demetTimer > 0 && ((millis() - demetTimer) > demetDelay)) 
      {
        digitalWrite(demetHD, LOW); // open demetra HD
        demetTimer = 0;
      }
      Rain();//#6
      Vine();//#7
      Dionis1();//#8
      Hercules();//#9
      if(hercuTimer > 0 && (tick - hercuTimer > HDDelay)) 
      {
        openHercuHD();
                         // this lock should be re-unlocked every 3 minutes after this moment
        hercuTimer = 0;
      }
      Narcis();//#10
      Thunder();//#11
    }
    // ==================================== SHIELDS ==================================
    if (!shieldDone)
    {
      Afina1();//#12
      if(afinaTimer > 0 && ((millis() - afinaTimer) > afinaHDdelay)) 
      {
        digitalWrite(afinaHD1, LOW);
        afinaTimer = 0;
      }
      Afina2();//#13
      TimeG();//#14
      Octopus();//#15
      Note();//#16
      if(noteTimer > 0 && (tick - noteTimer > noteHDDelay)) 
      {
        noteTimer = 0;
        digitalWrite(noteHD, LOW); // here note open wind box gives players wind power
      }
      Wind();//#17
      Ghera1();//#18
    }

    // ==================================== SEALS ==================================
    if (!sealsDone)
    {
      Fire();//#19
      Flower1();//#20
      Flower2();//#21
      if(flowerTimer > 0 && (tick - flowerTimer > flowerDelay)) 
      {
        flowerTimer = 0;
        digitalWrite(flowrHD, LOW); // players get seal 1
      }
      Dionis2();//#22
      if(dioni2Timer > 0 && (tick - dioni2Timer > dioni2Delay)) 
      {
        digitalWrite(dioniHD2, LOW); // open first dionis vault
        dioni2Timer = 0;
      }
      Arpha();//#23
      if(arphaTimer > 0 && (tick - arphaTimer > arphaDelay))
      {
        arphaTimer = 0;
        digitalWrite(arphaHD, LOW);  // give players seal 3
      }
      Ghera2();//#24
      /*
         String gadgetNames[32] = {0 "Baloon", 1 "Press", 2 "Gate",
                          3 "Poseidon", 4 "Trident", 5 "Demetra-1", 6 "Rain", 7 "Vine", 8 "Dionis-1", 9 "Hercules”, 10 “Narcis", 11 "Thunder",
                          12 "Afina-1", 13 "Afina-2", 14 "Time", 15 "Octopus", 16 "Note", 17 "Wind", 18 "Ghera-1",
                          19 "Fire", 20 "Flower-1", 21 "Flower-2", 22 "Arpha", 23 "Dionis-2", 24 "Ghera-2",
                          25 "Under",26 "Zodiak", 27 "Minot", 28 "Gorgona", 29 "Cristals", 30 "Light",
                          31 "End"
                         };
      */
    }
    // ==================================== CRYSTALS ==================================
    if (!passGStates[crystals] && sealsDone)
    { // underground level
      //OPEN UNDER
      BigKey();
      if (underRFWait) Underground();//#25
      else
      {
        Zodiak();//#26
        Minotavr();//#27
        Gorgona();//#28
        Crystals();//#29
      }
      if (tick % 35000 == 0) openOpened();  // open (and re-open if closed) underground locks
    } // eof_cristals
  } // eof.level_50

  if (level == 100)
  {
    Serial.println("WIN");
  }

  sendGStates(); // if need

  if (tick - lastA9SentTime > 10000) // send sync signal to confirm connection to bridge
  {
    digitalWrite(SSerialTxControl, HIGH);
    Serial1.write(0xA9);
    delay(10);
    digitalWrite(SSerialTxControl, LOW);
    Serial.println("Send sync signal");
    lastA9SentTime = tick;
  }
  voiceStates[1] = voiceStates[0];
  startStates[1] = startStates[0];
} // LOOP END




void sendToSlave(int address, byte data)
{
  Wire.beginTransmission(address); // transmit to device #8
  Wire.write(data);
  Wire.endTransmission();
}

void send250ms(int pin)
{
  digitalWrite(pin, HIGH);
  delay(250);
  digitalWrite(pin, LOW);
}

void skipNextGadget()
{
  int curGadget = 0;
  while (passGStates[curGadget]) {
    curGadget++;
  }
  passGStates[curGadget] = true;
  sendGStates();
  operGStates[curGadget] = true;
  passGStates[curGadget] = false;
  Serial.println("Gadget #(from 0): " + String(curGadget) + " Named: " + String(gadgetNames[curGadget]) + " passed by start button");
//  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("Skpd " + String(gadgetNames[curGadget]));
}

void voiceCurGadget()
{
  int curGadget = 0;
  while (passGStates[curGadget]) {
    curGadget++;
  }
  playVoice(++curGadget);
}

void playVoice(byte vhi)
{
  int playFile = (int)(vhi * 10) + voiceHintStates[vhi];
  Serial.println("Playing " + String(playFile) + ".mp3 file");
  mp3_set_serial(Serial3);
  delay(20);
  mp3_play(playFile); 
  delay(20);
  mp3_set_serial(Serial);
  delay(20);
 
  voiceHintStates[vhi] = (voiceHintStates[vhi] + 1) % 3;
}
