// Ago 09 2018
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

#define SSerialRX 10  //Serial Receive pin
#define SSerialTX 11  //Serial Transmit pin
#define SSerialTxControl 3   //RS485 Direction control

SoftwareSerial rs485(SSerialRX, SSerialTX); // RX, TX

byte devCount = 16

int startRF  = 8;    // RFID key to start
int underRF  = 9;    // RFID key to the underground door
int gateRF   = 10;   // RFID key to gate

ArdCPZ *cpz1;
ArdCPZ *cpz2;
ArdCPZ *cpz3;

//address i2c
lightConAddr = 20;
motorConAddr = 21;

//Gadget states
byte curGStates[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
byte newGStates[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
/*
 * 
1 Кнопка (Press)
2 Шар (Baloon)
3 Посейдон (Poseidon)
4 Деметра (Demetra)
5 Винодельня (VineMaker)
6 Дионис (Dionis)
7 Геркулес (Hercul)
8 Молния (Molni)
9 Нарцис (Narci)
10 Нот (Note)
11 Афина (Afina)
12 Осьминог (Octop)
13 Арфа (Arfa)
14 Цветок (Flower)
15 Muses (Muses)
16 Гера (Ghera)
 */
// lev 0
byte start = 0;


//
int startBeacon = 499;
int underBeacon = 328;
int gateBeacon  = 230;

byte baloon = 2;
int balloIN  = 50;   // a.
int balloOUT = 44;

byte presss = 1;
int pressIN  = 48;   // b.
int pressOUT = 42;

byte demetra = 4;
int demetIN  = 26;   // d.
int demetOUT = 38;
int demetHD  = A1;

byte vine = 5;
int vinemIN  = 24;   //e.
int vinemOUT = 36;

byte dionis = 6;
int dioniIN  = 22;   //f.
int dioniOUT = 34;
int dioniHD1 = A2;   // piston
int dioniHD2 = A3;

byte hercul = 7;
int hercuIN  = 7;
int hercuHD  = A4;

byte molni = 8;
int molniIN  = 52;   // g.
int molniOUT = 32;

byte poseidon = 3;
int poseiIN  = 53;   // h.
int poseiOUT = 43;

byte narcis = 9;
int narciIN  = 51;   // i.  /// video seen
int narciOUT = 41;          /// launch video

// lev 3
byte note = 10;
int noteIN   = 49;   // j.
int noteOUT  = 39;
int noteHD   = A6;

byte octopus = 12;
int octopIN  = 47;   // k.
int octopOUT = 37;

byte afina = 11;
int afinaIN  = 27;   // m.
int afinaOUT = 35;   //
int afinaHD1 = A7;
int afinaHD2 = A8;

/// level 4
byte muses = 15;
int musesIN  = 23;   // o. to change their messages
int musesOUT = 31;   // correct pattern

byte arpha = 13;
int arphaIN  = 6;
int arphaHD  = A9;

// level 5

int gorgoIN  = 28;   // A.
int gorgoOUT = 30;
int gorgoHD  = A12;

int minotIN  = 29;   // B.
int minotOUT = 45;
int minotHD  = A10;

int zodiaIN  = 5;
int zodiaHD  = A10;

byte flower = 14;
int flowrIN  = 25;
int flowrOUT = 33;   //n.
int flowrHD  = A13;

byte ghera = 16;
int gheraIN   = 4;
int gheraOUT  = 2;

//int worldIN   = 12; free
//int worldOUT  = 11; free

int level = 10;

bool balloPASS  = false;
bool pressPASS  = false;
bool gateOpen   = false;
bool startCard  = false;
bool underOpen  = false;
bool molniiDone = false;
bool shieldDone = false;
bool afinaHD1open = false;
bool sealsDone  = false;

bool seal1  = false;
bool seal2  = false;
bool seal3  = false;

bool crist1 = false;
bool crist2 = false;
bool crist3 = false;
bool cristaDone = false;

bool zodiaState = false;
bool minotState = false;
bool gorgoState = false;

bool flowerFirst = false;

void setup() {
  Serial.begin (9600);
  //Serial3.begin(9600);  //xbee
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


  pinMode( balloIN  , INPUT_PULLUP);
  pinMode( balloOUT , OUTPUT);

  pinMode( pressIN  , INPUT_PULLUP);
  pinMode( pressOUT , OUTPUT);

  pinMode( demetIN  , INPUT_PULLUP);
  pinMode( demetOUT , OUTPUT);
  pinMode( demetHD  , OUTPUT);

  pinMode( vinemIN  , INPUT_PULLUP);
  pinMode( vinemOUT , OUTPUT);    // click-lock on prop,

  pinMode( dioniIN  , INPUT_PULLUP);
  pinMode( dioniOUT , OUTPUT);
  pinMode( dioniHD1 , OUTPUT); // piston show up
  pinMode( dioniHD2 , OUTPUT);

  pinMode( hercuIN  , INPUT_PULLUP);
  pinMode( hercuHD  , OUTPUT);

  pinMode( molniIN  , INPUT_PULLUP);
  pinMode( molniOUT , OUTPUT);

  pinMode( poseiIN  , INPUT_PULLUP);
  pinMode( poseiOUT , OUTPUT);

  pinMode( narciIN  , INPUT_PULLUP);
  pinMode( narciOUT , OUTPUT);     /// launch video

  pinMode( noteIN   , INPUT_PULLUP);
  pinMode( noteOUT  , OUTPUT);
  pinMode( noteHD   , OUTPUT);

  pinMode( octopIN  , INPUT_PULLUP);
  pinMode( octopOUT , OUTPUT);    // click-lock on prop

  pinMode( afinaIN  , INPUT_PULLUP);
  pinMode( afinaOUT , OUTPUT);

  pinMode( afinaHD1 , OUTPUT);
  pinMode( afinaHD2 , OUTPUT);

  pinMode( musesIN  , INPUT_PULLUP);
  pinMode( musesOUT , OUTPUT);

  pinMode( arphaIN  , INPUT_PULLUP);
  pinMode( arphaHD  , OUTPUT);

  pinMode( gorgoIN  , INPUT_PULLUP);
  pinMode( gorgoOUT , OUTPUT);
  pinMode( gorgoHD  ,  OUTPUT);  //click

  pinMode( minotIN  , INPUT_PULLUP);
  pinMode( minotOUT , OUTPUT);
  pinMode( minotHD  , OUTPUT); // click

  pinMode( zodiaIN  , INPUT_PULLUP);
  pinMode( zodiaHD  , OUTPUT); //click

  pinMode( flowrIN  , INPUT_PULLUP);
  pinMode( flowrOUT , OUTPUT);

  pinMode( gheraIN  , INPUT_PULLUP);
  pinMode( gheraOUT , OUTPUT);

  //pinMode( worldIN  , INPUT_PULLUP);
  //pinMode( worldOUT , OUTPUT);

  cpz1 = new ArdCPZ(startRF);
  cpz2 = new ArdCPZ(underRF);
  cpz3 = new ArdCPZ(gateRF);

  pinMode(SSerialTxControl, OUTPUT);
  digitalWrite(SSerialTxControl, LOW);  // Init Recieve RS485
  //I2C Start
  Wire.begin();
  Serial.println("I2C Started.")
  delay(10);
  rs485.begin(9600);
  Serial.println("RS485 Started.")
  delay(10);
  checkInputs();
  Serial.println("Inputs Checked.")
  delay(10);
  openLocks();
  Serial.println("Locks Opened.")
}

void loop() {
  getOperSkips();
  if (level == 10) // START
  {
    if (millis() % 330 == 0)
    {
      if (checkStartRFID())
      {
        start += 1;   // wait for prestart signal ( any RFID from startRF )
        if (start == 1)
        {
          // if received send command to Motor_controller and Light_controller
          mp3_play(2); // prestart message
          delay(200);
          //commands to light and motor controllers
          sendToSlave(lightConAddr, 0x01); // выключаем весь свет
          delay(10);
          sendToSlave(motorConAddr, 0x01); // спускает штору [column up] , поднимает облако [cloud up], поднимает колонну[column up],
                                           // поднимают виноград [grape up], шторку опускаем [courtainDN]

        }
        else if (start == 2)
        {
          mp3_play(3); // start message
          //start game
          delay(200);
          //send random wind to lightController
          sendToSlave(lightConAddr, 0x01); // random Wind
          level = 20;
        }
      }
    }
  }

  if (level = 20)  {
    if (!digitalRead(balloIN) || currGStates[baloon] == 1) //signal from finished ballon received
    {
      newGStates[baloon] = 1;
      Serial.println("Ballon signal recieved. Turn on the lights ad roll up the curtain");
      // send (i2c) signal to motor_controller >rollUp and light_controller  >lights and stop wind
      level = 30;
    }
    // if players never finish ballon, operator can skip it here (send signal to ballon)
  }

  if (level = 30)  {
    if (!digitalRead(pressIN) || currGStates[presss] == 1) //victory signal from press received
    {
      Serial.println("Press signal recieved. Press gave players the  RFID key to the gate ");
      level = 40;
    }
    // if players never finish press, operator can skip it here (send signal to press)
  }

  if (level = 40)  {
    if (gateOpen == false) {
      if (millis() % 300 == 0) {  // every 300ms check RFID
        checkRFID();      // wait for signal from gateRF(ID) (cpz3)
      }
    } else {  // if gateRFIDkey is readed and gate(is)Open
      // send signal to motor_controller >openGate > gheraLevel = 1 > and start Ghera > open HD1
      level = 50;
      signal2ghera();  // ghera start speaking, 'molnii' level
    }
    // if players never find the key, operator can skip it here > gateOpen = true;
  }

  if (level = 50)  { // on level 50 all events may be done in any order/sequence

    // ---------molnii--------------
    if (molniiDone == false ) {

      if (digitalRead(poseiIN) == LOW) {  // signal from poseidon
        // posei > command via (i2c) to motor_controller activate falling column > players get first part molnii
        // shoud be skippable from master console
      }

      if (digitalRead(demetIN) == LOW) {  // signal from demetra    ?????????????????????????????
        digitalWrite(demetHD , LOW); // open demetra HD
        // shoud be skippable from master console
      }

      // demetra gives players the water > they should put it int the World
      // that will activate Grape grow , if players take grape an put it in the  - vineMkr
      // players will get vine wich they can use to fill bottle for dionis
      // world send signal (i2c) to motor_controller to grape grow


      if (digitalRead(dioniIN) == LOW && molniiDone == false) {      // if players gives dionis empty bottle
        // he will tell them that he dont like empty bottles
        // if signal from full bottle is received >
        // dioniHD1 opens > players gets second part molnii
        digitalWrite(dioniHD1, LOW); // open first dionis vault
        // shoud be skippable from master console
      }

      if (digitalRead(hercuIN) == LOW ) {      // hercu > players gets third part of molnii
        digitalWrite(hercuHD, LOW);
        // shoud be skippable from master console
      }

      if (digitalRead(narciIN) == LOW ) {      // narci > pattern information for players
        // nothing happen - narcis only give players information
        // shoud be triggerable from master console
      }

      if (digitalRead(molniIN) == LOW) {
        //if all molnii are done >   // gheraLevel = 2 >  speaks > opend HD2 (shields)
        signal2ghera();  // moves ghera to 'shields' level
        molniiDone = true;
        // may add some extra storm effects thru light_controller
        // shoud be triggerable from master console
      }
    }// eof.molniiDone

    //-------- shields ------
    if ( shieldDone == false ) {

      if (digitalRead(afinaIN) == LOW && afinaHD1open == false) {  // afina first signal opens afinaHD1
        digitalWrite(afinaHD1, LOW);
        while (digitalRead(afinaIN) == LOW) { }
        delay(300);
        // here afina gives players a key to open next vault
      }

      if (digitalRead(afinaIN) == LOW && afinaHD1open == true) {  // afina  second signal > open afinaHD2  > players get escu1
        digitalWrite(afinaHD2, LOW);
        // here afina gives players another part of the shield

      }

      if (digitalRead(octopIN) == LOW) {     // octop done > players get escu2
        // octopus has its own hideout, so nothing happens here
        // octopus gives players another part of the shield
      }

      if (digitalRead(noteIN) == LOW) {       // note > players get escu3
        digitalWrite(noteHD, LOW);
      }
      // note gives players the wind element > they should put it int the World
      // wind will activate fastled backlight on world and windSensor
      // if players blow in the sensor World wil send signal to master, master will send signal to motor_controller (windBlow)
      // motor_controller will activate (cloudDOWN)
      // players will get part of the shield , that they shoud give to ghera later

      if (digitalRead(gheraIN) == LOW && shieldDone == false) { //if all shields in ghera place (gheraLevel = 3 ) gera speaks - open HD3 (seals)
        shieldDone = true;
        signal2muses();
        // ghera is on 'seals' level
      }
    } //eof_shields

    //---------seals --------
    if (sealsDone == false) {

      if (digitalRead(flowrIN) == LOW && flowerFirst == false) {  // first level of flower
        while (digitalRead(flowrIN) == LOW) { }
        delay(50);
      }

      if (digitalRead(flowrIN) == LOW && flowerFirst == true) {   // second level of flower
        while (digitalRead(flowrIN) == LOW) { }
        delay(50);
        digitalWrite(flowrHD, LOW); // players get seal 1
      }

      if (digitalRead(dioniIN) == LOW) {    // dionis(2) cold heart if all done
        digitalWrite(dioniHD2, LOW);  // open second dionis vault > players get seal 2
      }

      if (digitalRead(arphaIN) == LOW) {    //  signal from arpha received
        signal2muses();  // send signal to shut up the muses
        digitalWrite(arphaHD, LOW);  // give players seal 3
      }

      // if all seals are in ghera place  > gheraLevel send signal to master  >>   sealsDone = true;
      // ((((  ghera speaks > open HD4 (key for underground) ))))
      //
      // master send signal to motor_controller(COLUMNS_UP) to shift columns up
      // and send signal to motor_controller to raise up the columns

    }// eof_seals

    //---------cristals------
    if (cristaDone == false ) {  // underground level
      if (digitalRead(zodiaIN) == false ) {     // if zodia done > players get cryst1
        // shoud be skippable from master console
        // click lock
        zodiaState = true;
      }

      if (digitalRead(minotIN) == false ) {     // if minotavr done > players get cryst2
        // shoud be skippable from master console
        // click lock
        minotState = true;
      }

      if (digitalRead(gorgoIN) == false ) {     // if gorgona done > players get cryst3
        // shoud be skippable from master console
        // click lock
        gorgoState = true;
      }


      /// missing part -- missing pins  -- run on 3 RFIDs on motor_citr or light_contr
      /// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      /// they just received the cristals , now they have to install them in a correct place all three
      //  crist1 = true;
      //  crist2 = true;
      //  crist3 = true;
      /// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

      if (crist1 && crist2 && crist3) {  //if all crystals are in place > give shoe  >>> final
        level = 100;
        cristaDone = true;
        signal2ghera();
      }

      if (millis() % 5000 == 0) openOpened();  // open (and re-open if closed) underground locks
    } // eof_cristals





  } // eof.level_50

  if (level == 100) {
    // game over - victory !
  }
  if (checkDevices()) sendStates();
}

void sendToSlave(int address, byte data)
{
  Wire.beginTransmission(address); // transmit to device #8
  Wire.write(data);
  Wire.endTransmission();
}
//I2C Request
/*
   byte bufercnt = Wire.requestFrom(rfidSlaveAddr[i], 1);
        if (bufercnt == 1)
        {
          currVals[i] = Wire.read();
          part = ((currVals[i] * 1.0) / 10.0) * 100.0; //TEST
          if (part == 100 && !RFIDClosed[i]) {
            digitalWrite(RFIDBlockRelays[i], LOW);
            RFIDClosed[i] = true;
          }
        }
*/
