// Ago 30.01.2018
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

//I2C PINS: 20-21

#define SSerialTxControl 17   //RS485 Direction control

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
boolean operGStates[31];
boolean passGStates[31];

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
String gadgetNames[31] = {"Baloon", "Press", "Gate", "Poseidon", "Trident", "Demetra-1", "Rain", "Vine", "Dionis-1", "Hercules”, “Narcis", "Thunder", "Afina-1", "Afina-2", "Time", "Octopus", "Note", "Wind", "Ghera-1", "Fire", "Flower-1", "Flower-2", "Arpha", "Dionis-2", "Ghera-2", "Zodiak", "Minot", "Gorgona", "Cristals", "Light", "End"};
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
int gateBeacon  = 384;
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
byte dionis2 = 22;
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
byte arpha = 23;
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

int totalGadgets = 31;

int freeIN = 46;
int freeOUT = 40;
//int worldIN   = 12; free
//int worldOUT  = 11; free

byte level = 10;
//boolean gateOpen   = false; need?
boolean shieldDone = false;
boolean thunderDone = false;

boolean seal1  = false;
boolean seal2  = false;
boolean seal3  = false;
boolean sealsDone  = false;

unsigned long startHighPin = 0;

void setup() {
  Serial.begin (9600);
  //Serial3.begin(9600);  //xbee
  Serial1.begin(9600);  //RS-485 adapter

  Serial.println("\nSKY Master v2 ");
  Serial.println("5 Sep 2018");

  delay(100);
  mp3_set_serial(Serial);
  delay(100);
  mp3_set_volume (28);
  delay(100);
  mp3_play(1); // test startup sound
  delay(200);

  Serial.println("\n--------------");

  pinMode(startPin, INPUT_PULLUP);
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

  pinMode( thundIN  , INPUT_PULLUP);
  pinMode( thundOUT , OUTPUT);

  pinMode( poseiIN  , INPUT_PULLUP);
  pinMode( poseiOUT , OUTPUT);
  pinMode( poseiHD , OUTPUT);

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
  pinMode( flowrHD,   OUTPUT);
  //pinMode( worldIN  , INPUT_PULLUP);
  //pinMode( worldOUT , OUTPUT);

  cpz1 = new ArdCPZ(windRFPin);
  cpz2 = new ArdCPZ(underRFPin);
  cpz3 = new ArdCPZ(gateRFPin);
  cpz4 = new ArdCPZ(rainRFPin);

  for (int g = 0; g < totalGadgets; g++)
  {
    operGStates[g] = false;
    passGStates[g] = false;
  }

  pinMode(SSerialTxControl, OUTPUT);
  digitalWrite(SSerialTxControl, LOW);  // Init Recieve RS485
  //I2C Start
  Wire.begin();
  Serial.println("I2C Started.");
  delay(10);
  Serial.println("RS485 Started.");
  delay(10);
  checkInputs();
  Serial.println("Inputs Checked.");
  delay(10);
  openLocks();
  Serial.println("Locks Opened.");
}

void loop()
{
  startStates[0] = debounce(startStates[1], startPin); // READ START BUTTON
  
  if(level > 10 && !startStates[0] && startStates[1]) // SKIP BY START BUTTON
  {
    int curGadget = 0;
    while(passGStates[curGadget]) {curGadget++;}
    operGStates[curGadget] = true;
    Serial.println("Gadget #" + String(gadgetNames[curGadget]) + " passed by start button");
  }
  
  if (curHighPin > 0) // WATCH FOR ACTIVE PIN
  {
    if (millis() - startHighPin > 250)
    {
      digitalWrite(curHighPin, LOW);
      curHighPin = -1;
    }
  }
  
  getOperSkips();
  
  if (level == 10) // START
  {
    if (!startStates[0] && startStates[1])
    {
      start ++;   // wait for prestart signal ( any RFID from startRF )
      if (start == 1)
      {
        // if received send command to Motor_controller and Light_controller
        mp3_play(2); // prestart message
        delay(200);
        //commands to light and motor controllers
        sendToSlave(lightConAddr, 0x10); // выключаем весь свет
        delay(10);
        sendToSlave(motorConAddr, 0x10); // поднимает облако [cloud up], поднимает колонну[column up],
        // поднимают виноград [grape up]
      }
      else if (start == 2)
      {
        mp3_play(3); // start message
        //start game
        delay(200);
        //send random wind to lightController
        sendToSlave(lightConAddr, 0x11); // random Wind
        Serial.println("Go to level 20");
        level = 12;
      }
    }    
  }
  else if (level == 12)
  {
    if ((!digitalRead(balloIN) || operGStates[baloon]) && !passGStates[baloon]) //signal from finished ballon received
    {
      passGStates[baloon] = true;
      if (operGStates[baloon]) send250ms(balloOUT);
      Serial.println("Ballon signal recieved. Turn on the lights ad roll up the curtain");
      // send (i2c) signal to motor_controller >rollUp and light_controller  >lights and stop wind
      sendToSlave(lightConAddr, 0x12); // wind off
      delay(10); // replace between
      sendToSlave(motorConAddr, 0x12);
      send250ms(pressOUT);
      level = 13;
      Serial.println("Go to level 30");
    }
    // if players never finish ballon, operator can skip it here (send signal to ballon)
  }
  else if (level == 13)
  {
    if ((!digitalRead(pressIN) || operGStates[presss]) && !passGStates[presss]) //victory signal from press received
    {
      passGStates[presss] = true;
      if (operGStates[presss]) send250ms(pressOUT);
      Serial.println("Press signal recieved. Press gave players the RFID key to the gate ");
      level = 14;
      Serial.println("Go to level 40");
    }
    // if players never finish press, operator can skip it here (send signal to press)
  }
  else if (level == 14)
  {
    gateRFWait = !getGateRFID();
    Serial.println("gateWait = " + String(gateRFWait));
    Serial.println("gatePassState = " + String(passGStates[gate]));
    if ((!gateRFWait || operGStates[gate]) && !passGStates[gate])
    {
      passGStates[gate] = true;
      sendToSlave(motorConAddr, 0x13); // send signal to motor_controller >openGate
      //send250ms(gheraOUT);  // ghera start speaking, 'thunder' level
      //door1 10s отдельно
      level = 90;
      Serial.println("Go to level 50");
      //START MP3 FILE
    }// if players never find the key, operator can skip it here > gateOpen = true;
  }
  else if (level == 90)
  { // on level 50 all events may be done in any order/sequence
    //Big request to World
    if (millis() % 100 == 0)
    {
      if (getWindRFID() && windRFWait) windRFWait = false;
      if (getRainRFID() && rainRFWait) rainRFWait = false;
      if (getUnderRFID() && underRFWait) underRFWait = false;
    }
    // --------------------
    // ---------thunder--------------
    if (!thunderDone)
    {
      if ((!digitalRead(poseiIN) || operGStates[poseidon]) && !passGStates[poseidon])
      { // signal from poseidon
        // posei > command via (i2c) to motor_controller activate falling column > players get first part thundi
        // shoud be skippable from master console
        passGStates[poseidon] = true;
        if (operGStates[poseidon]) send250ms(poseiOUT);
        digitalWrite(poseiHD, LOW); // Открываем тайник Посейдона, даем игрокам трезубец
        Serial.println("Poseidon Done");
      }

      if ((!digitalRead(triPin) || operGStates[trident]) && !passGStates[trident] && passGStates[poseidon])
      {
        sendToSlave(motorConAddr, 0x21); // Column Down
        // START MP3 - FILE
        passGStates[trident]=true;
      }


      if ((!digitalRead(demetIN) || operGStates[demetra]) && !passGStates[demetra])
      { // shoud be skippable from master console
        // signal from demetra    ?????????????????????????????
        passGStates[demetra] = true;
        if (operGStates[demetra]) send250ms(demetOUT);
        digitalWrite(demetHD, LOW); // open demetra HD
        Serial.println("Demetra 1 part Done");
      }

      if ((!rainRFWait || operGStates[rain]) && !passGStates[rain] && passGStates[demetra])
      {
        sendToSlave(motorConAddr, 0x22); // send signal to motor_controller > grapeGrow
        // MP3 FILE
      }

      if ((!digitalRead(vinemIN) || operGStates[vine]) && !passGStates[vine] && passGStates[demetra] && passGStates[rain])
      {
        passGStates[vine] = true;
        send250ms(dioniOUT);
        Serial.println("Demetra 2 part Done");
      }

      if ((!digitalRead(dioniIN) || operGStates[dionis1]) && !passGStates[dionis1] && passGStates[vine])
      { // shoud be skippable from master console
        // if players gives dionis empty bottle
        // he will tell them that he dont like empty bottles
        // if signal from full bottle is received >
        // dioniHD1 opens > players gets second part thundi
        passGStates[dionis1] = true;
        digitalWrite(dioniHD1, LOW); // open first dionis vault
        // MP3 FILE
        Serial.println("Demetra Done");
      }

      // demetra gives players the water > they should put it int the World
      // that will activate Grape grow , if players take grape an put it in the  - vineMkr
      // players will get vine wich they can use to fill bottle for dionis
      // world send signal (i2c) to motor_controller to grape grow

      if ((!digitalRead(hercuIN) || operGStates[hercul]) && !passGStates[hercul])
      { // shoud be skippable from master console
        // hercu > players gets third part of thundi
        passGStates[hercul] = true;
        digitalWrite(hercuHD, LOW);
        Serial.println("Hercules Done");
      }

      if ((!digitalRead(narciIN) || operGStates[narcis]) && !passGStates[narcis])
      { // narci > pattern information for players
        // nothing happen - narcis only give players information
        // shoud be triggerable from master console
        if (operGStates[narcis]) send250ms(narciOUT);
        passGStates[narcis] = true;
        Serial.println("Narcis Done");
      }

      if ((!digitalRead(thundIN) || operGStates[thunder]) && !passGStates[thunder])
      { // shoud be triggerable from master console
        // may add some extra storm effects thru light_controller
        //if all thundi are done >   // gheraLevel = 2 >  speaks > opend HD2 (shields)
        send250ms(gheraOUT);  // moves ghera to 'shields' level, ALWAYS OR BY OPERATOR?
        passGStates[thunder] = true;
        thunderDone = true;
        Serial.println("thunder Done");
      }
    }// eof.thundiDone

    //-------- shields ------
    if (!shieldDone)
    {
      if ((!digitalRead(afinaIN) || operGStates[afina1]) && !passGStates[afina1])
      { // afina first signal opens afinaHD1
        digitalWrite(afinaHD1, LOW); // here afina gives players a key to open next vault
        passGStates[afina1] = true;
        Serial.println("Afina 1 part Done");
      }

      if ((!digitalRead(afinaIN) || operGStates[afina1]) && !passGStates[afina2] && passGStates[afina1])
      { // afina  second signal > open afinaHD2  > players get escu1
        digitalWrite(afinaHD2, LOW); // here afina gives players another part of the shield
        passGStates[afina2] = true;
        Serial.println("Afina 2 part Done");
      }

      if ((!digitalRead(timeIN) || operGStates[Time]) && !passGStates[Time])
      {
        if (operGStates[Time]) send250ms(timeOUT);
        passGStates[Time] = true;
        Serial.println("Time part Done");
      }

      if ((!digitalRead(octopIN) || operGStates[octopus]) && !passGStates[octopus] && passGStates[Time])
      { // octop done > players get escu2
        // octopus has its own hideout, so nothing happens here
        // octopus gives players another part of the shield
        //if (operGStates[Time]) send250ms(timeOUT); Like this
        if(operGStates[octopus]) send250ms(octopOUT);
        passGStates[octopus] = true;
        Serial.println("Octopus Done");
      }

      if ((!digitalRead(noteIN) || operGStates[note]) && !passGStates[note])
      {
        // note > players get escu3
        if (operGStates[note]) send250ms(noteOUT);
        // MP3 FILE
        Serial.println("Note Done");
        passGStates[note] = true;
      }
      
      if ((!windRFWait || operGStates[wind]) && !passGStates[wind] && passGStates[note]) //disc
      {//Из тайника игроки получают рфид ветра.и когда вставляют его в приемник рфида ветра то включаем ветер на 10-15 секунд, 
        //мр3 файл и спускаем облакоИз тайника игроки получают рфид ветра.и когда вставляют его в приемник рфида 
        //ветра то включаем ветер на 10-15 секунд, мр3 файл и спускаем облако
        passGStates[wind] = true;
        Serial.println("Wind RFID Recieved");
        sendToSlave(motorConAddr, 0x31); // CloudDown - отдать 3 часть щита
        sendToSlave(lightConAddr, 0x31); // windBlow 10-15 secs
        // MP3 FILE
      }

      if ((!digitalRead(gheraIN) || operGStates[ghera1]) && !passGStates[ghera1])
      { //if all shields in ghera place (gheraLevel = 3 ) gera speaks - open HD3 (seals)
        // ghera is on 'seals' level
        passGStates[ghera1] = true;
        send250ms(musesOUT);
        send250ms(gheraOUT);
        Serial.println("SHIELD Done");
      }
    }

    // note gives players the wind element > they should put it int the World
    // wind will activate fastled backlight on world and windSensor
    // if players blow in the sensor World wil send signal to master, master will send signal to motor_controller (windBlow)
    // motor_controller will activate (cloudDOWN)
    // players will get part of the shield , that they shoud give to ghera later

    //eof_shields

    //---------seals --------печати (ДОПИСЫВАТЬ ОТСЮДА)
    if (!sealsDone)
    {
      if (!digitalRead(firePin) && !fireState)
      {
        // turner start
        sendToSlave(lightConAddr, 0x41); //turner start
        fireState = true;
      }

      if ((!digitalRead(flowrIN) || operGStates[flower1]) && !passGStates[flower1])
      { // first level of flower
        if (operGStates[flower1]) send250ms(flowrOUT);
        passGStates[flower1] = true;
      }

      if ((!digitalRead(flowrIN) || operGStates[flower1]) && passGStates[flower1] && !passGStates[flower2])
      { // second level of flower
        delay(50);
        digitalWrite(flowrHD, LOW); // players get seal 1
        sendToSlave(lightConAddr, 0x42); // turner off, light switch
        if (operGStates[flower2]) send250ms(flowrOUT);
        passGStates[flower2] = true;
      }

      if ((!digitalRead(dioniIN) || operGStates[dionis2]) && !passGStates[dionis2])
      { // dionis(2) cold heart if all done
        digitalWrite(dioniHD2, LOW);  // open second dionis vault > players get seal 2
        passGStates[dionis2] = true;
      }

      if ((!digitalRead(arphaIN) || operGStates[arpha]) && !passGStates[arpha])
      { //  signal from arpha received
        send250ms(musesOUT); // send signal to shut up the muses
        digitalWrite(arphaHD, LOW);  // give players seal 3
        passGStates[arpha] = true;
      }
      if (passGStates[flower2] && passGStates[arpha] && passGStates[dionis2])
      {
        if ((!digitalRead(gheraIN) || operGStates[ghera2]) && !passGStates[ghera2])
        {
          if(operGStates[ghera2]) send250ms(gheraOUT);
          passGStates[ghera2] = true;
          sealsDone = true;
          // if all seals are in ghera place  > gheraLevel send signal to master  >>   sealsDone = true;
          // ((((  ghera speaks > open HD4 (key for underground) ))))
        }
      }
      //OPEN UNDER
    }// eof_seals

    //---------cristals------
    if (!passGStates[crystals])
    { // underground level
      
      if ((!underRFWait || operGStates[under]) && !passGStates[under])
      {
        sendToSlave(motorConAddr, 0x51);
        passGStates[under] = true;
      }
      
      if ((!digitalRead(zodiaIN) || operGStates[zodiak]) && !passGStates[zodiak] && passGStates[under])
      { // if zodia done > players get cryst1
        // shoud be skippable from master console
        // click lock
        passGStates[zodiak] = true;
      }

      if ((!digitalRead(minotIN) || operGStates[minot]) && !passGStates[minot] && passGStates[under])
      { // if minotavr done > players get cryst2
        // shoud be skippable from master console
        // click lock
        passGStates[minot] = true;
      }

      if ((!digitalRead(gorgoIN) || operGStates[gorgona]) && !passGStates[gorgona] && passGStates[under])
      { // if gorgona done > players get cryst3
        // shoud be skippable from master console
        // click lock
        passGStates[gorgona] = true;
      }

      if (passGStates[gorgona] && passGStates[minot] && passGStates[zodiak])
      {
        if (crystReciever)
        {
          Wire.requestFrom(lightConAddr, 1);
          byte lightUnsw = Wire.requestFrom(lightConAddr, 1, false);
          boolean crystSum = true;
          if (lightUnsw == 1)
          {
            byte unswer = Wire.read();
            for (int u = 0; u < 3; u++)
            {
              crystStates[u] = 1 & (unswer >> 2 * u);
              crystSum &= crystStates[u];
            }
            if (crystSum) passGStates[crystals] = true;
          }
          if (passGStates[crystals])
          {
            level = 100;
            //column up motor add!!!!!
            sendToSlave(motorConAddr, 0x99);
            send250ms(gheraOUT);
          }
        }
        else
        {
          if(!digitalRead(crystRecBut)) 
          {
            //Open Cryst Reciever throw lightController
            sendToSlave(lightConAddr, 0x55);
            crystReciever = true;
          }
        }
      }

      if (millis() % 5000 == 0) openOpened();  // open (and re-open if closed) underground locks
    } // eof_cristals
  } // eof.level_50

  if (level == 100)
  {
    // game over - victory !
  }
  sendGStates();
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

void send250ms2(int pin)
{
  if (curHighPin > 0) return;
  curHighPin = pin;
  digitalWrite(curHighPin, HIGH);
  startHighPin = millis();
}
