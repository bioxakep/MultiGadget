// 22.10.2018
// Rewrote Connection

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

byte devCount = 21;//Under calculate volitile gadgets...

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
String gadgetNames[32] = {"Baloon", "Press", "Gate",
                          "Poseidon", "Trident", "Demetra-1", "Rain", "Vine", "Dionis-1", "Hercules", "Narcis", "Thunder",
                          "Afina-1", "Afina-2", "Time", "Octopus", "Note", "Wind", "Ghera-1",
                          "Fire", "Flower-1", "Flower-2", "Arpha", "Dionis-2", "Ghera-2",
                          "Under","Zodiak", "Minot", "Gorgona", "Cristals", "Light",
                          "End"
                         };
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
boolean tridentWait = true;

//DEMETRA
byte demetra = 5;
int demetIN  = 26;   // d.
int demetOUT = 38;
int demetHD  = A1;

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
int dioniHD1 = A2;   // piston
int dioniHD2 = A3;

//HERCULES
byte hercul = 9;
int hercuIN  = 7;
int hercuHD  = A4;

//NARCIS
byte narcis = 10;
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
int afinaHD2 = A8;

//TIME
byte Time = 14;
int timeOUT = A5;
int timeIN = 46;

//OCTOPUS
byte octopus = 15;
int octopIN  = 47;   // k.
int octopOUT = 37;

//NOTE
byte note = 16;
int noteIN   = 49;   // j.
int noteOUT  = 39;
int noteHD   = A6;

//WIND
byte wind = 17;
ArdCPZ *cpz1; //Wind
int windBeacon = 499;
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

//ARPHA
byte arpha = 22;
int arphaIN  = 6;
int arphaHD  = A9;

//UNDERGROUND
byte under = 25;
ArdCPZ *cpz2; //Under
int underBeacon = 328;
boolean underRFWait = true;

//ZODIAK
byte zodiak = 26;
int zodiaIN  = 5;
int zodiaHD  = A11;

//MINOT
byte minot = 27;
int minotIN  = 29;   // B.
int minotOUT = 45;
int minotHD  = A10;

//GORGONA
byte gorgona = 28;
int gorgoIN  = 28;   // A.
int gorgoOUT = 30;
int gorgoHD  = A12;

//CRISTALS
byte crystals = 29;
boolean crystStates[3] = {false, false, false};
boolean crystDone = false;
boolean crystReciever = false;
int crystRecBut = 16;

byte light = 30;
byte win = 31;

int totalGadgets = 32;

int freeIN = 46;
int freeOUT = 40;
//int worldIN   = 12; free
//int worldOUT  = 11; free

byte level = 10;

boolean shieldDone = false;
boolean thunderDone = false;

boolean seal1  = false;
boolean seal2  = false;
boolean seal3  = false;
boolean sealsDone  = false;

boolean bridgeConnected = false;
unsigned long startHighPin = 0;
unsigned long lastA9SentTime = 0;
unsigned long lastRFIDCheck = 0;

void setup()
{
  Serial.begin (9600);
  delay(10);
  Serial1.begin(9600);  //RS-485 to Bridge
  delay(10);
  Serial.println("SKY Master v2.1");
  Serial.println("18 Okt 2018");
  Serial.println("RS485 Started.");
  pinSetup(); //from pinWorker
  
  delay(100);
  mp3_set_serial(Serial);
  delay(100);
  mp3_set_volume (28);
  delay(100);
  mp3_play(1); // test startup sound
  delay(200);

  Serial.println("\n--------------");

  cpz1 = new ArdCPZ(windRFPin);
  cpz2 = new ArdCPZ(underRFPin);
  cpz3 = new ArdCPZ(gateRFPin);
  cpz4 = new ArdCPZ(rainRFPin);
  Serial.println("RFID Connected");
  for (int g = 0; g < totalGadgets; g++)
  {
    operGStates[g] = false;
    passGStates[g] = false;
  }
  
  //I2C Start
  Wire.begin();
  Serial.println("I2C Started");
  
  delay(10);
  checkInputs();
  Serial.println("Inputs Checked");
  delay(10);
  openLocks();
  Serial.println("Locks Opened");
  lcd.init();
  lcd.backlight();
  lcd.print("Initialising...");
  
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
  if (level > 10 && !startStates[0] && startStates[1]) skipNextGadget(); // SKIP BY START BUTTON
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
  { // on level 50 all events may be done in any order/sequence
    //Big request to World
    if (tick - lastRFIDCheck > 100)
    {
      lastRFIDCheck = tick;
      if (windRFWait) windRFWait = !getWindRFID();
      if (rainRFWait) rainRFWait = !getRainRFID();
    }

    // ==================================== THUNDER ==================================
    if (!thunderDone)
    {
      Poseidon();//#3
      Trident();//#4
      Demetra();//#5
      Rain();//#6
      Vine();//#7
      Dionis1();//#8
      // demetra gives players the water > they should put it int the World
      // that will activate Grape grow , if players take grape an put it in the  - vineMkr
      // players will get vine wich they can use to fill bottle for dionis
      // world send signal (i2c) to motor_controller to grape grow
      Hercules();//#9
      Narcis();//#10
      Thunder();//#11
    }
    // ==================================== SHIELDS ==================================
    if (!shieldDone)
    {
      Afina1();//#12
      Afina2();//#13
      TimeG();//#14
      Octopus();//#15
      Note();//#16
      Wind();//#17
      Ghera1();//#18
    }
    // note gives players the wind element > they should put it int the World
    // wind will activate fastled backlight on world and windSensor
    // if players blow in the sensor World wil send signal to master, master will send signal to motor_controller (windBlow)
    // motor_controller will activate (cloudDOWN)
    // players will get part of the shield , that they shoud give to ghera later
    //eof_shields

    // ==================================== SEALS ==================================
    if (!sealsDone)
    {
      Fire();//#19
      Flower1();//#20
      Flower2();//#21
      Dionis2();//#22
      Arpha();//#23
      Ghera2();//#24
      /*
       * String gadgetNames[32] = {0 "Baloon", 1 "Press", 2 "Gate",
                          3 "Poseidon", 4 "Trident", 5 "Demetra-1", 6 "Rain", 7 "Vine", 8 "Dionis-1", 9 "Hercules”, 10 “Narcis", 11 "Thunder",
                          12 "Afina-1", 13 "Afina-2", 14 "Time", 15 "Octopus", 16 "Note", 17 "Wind", 18 "Ghera-1",
                          19 "Fire", 20 "Flower-1", 21 "Flower-2", 22 "Arpha", 23 "Dionis-2", 24 "Ghera-2",
                          25 "Under",26 "Zodiak", 27 "Minot", 28 "Gorgona", 29 "Cristals", 30 "Light",
                          31 "End"
                         };
       */
    }
    // ==================================== CRYSTALS ==================================
    if (!passGStates[crystals])
    { // underground level
      //OPEN UNDER
      if (underRFWait) Underground();//#25
      else
      {
        Zodiak();//#26
        Minotavr();//#27
        Gorgona();//#28
        Crystals();//#29
      }
      if (tick % 5000 == 0) openOpened();  // open (and re-open if closed) underground locks
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
  digitalWrite(13, HIGH);
  digitalWrite(pin, HIGH);
  delay(250);
  digitalWrite(pin, LOW);
  digitalWrite(13, LOW);
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
  Serial.println("Gadget #(from 0): "+ String(curGadget) + " Named: " + String(gadgetNames[curGadget]) + " passed by start button");
  lcd.clear();
  lcd.print("SKIPPED " + String(curGadget));
}

