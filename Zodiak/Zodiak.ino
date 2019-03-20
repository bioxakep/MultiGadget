//ArdCPZ_getAddress_x4
// june 4 2018 original
// june 5 2018 all added & tested OK - output & victory pending

#include <OneWire.h>
#include <ArdCPZ.h>

#define PIN_CPZ1 8
#define PIN_CPZ2 7
#define PIN_CPZ3 6
#define PIN_CPZ4 5
#define PIN_CPZ5 4
#define PIN_CPZ6 3
#define PIN_CPZ7 2

ArdCPZ *cpz1;
ArdCPZ *cpz2;
ArdCPZ *cpz3;
ArdCPZ *cpz4;
ArdCPZ *cpz5;
ArdCPZ *cpz6;
ArdCPZ *cpz7;

int ficha1ok = 499;
int ficha2ok = 328;
int ficha3ok = 230;
int ficha4ok = 220;
int ficha5ok = 328;
int ficha6ok = 230;
int ficha7ok = 220;

int sound  = 10;
int output = 11;
int led    = 13;

void setup() 
{
  Serial.begin(9600);
  Serial.println("\n\n Reading Beacons from A0 A1 A2 A3 and printing summs ");
  Serial.println(" and compares results vs saved data then print check ");
  Serial.println(" file name ArdCPZ_getAddress_x4.ino ");

  pinMode(output,OUTPUT);
  pinMode(led   ,OUTPUT);
  pinMode(sound ,OUTPUT);
 
  digitalWrite(output,LOW);
  digitalWrite(led,   LOW);
 
  digitalWrite(sound,LOW);
  tone(sound,900,900);

  cpz1 = new ArdCPZ(PIN_CPZ1);
  cpz2 = new ArdCPZ(PIN_CPZ2);
  cpz3 = new ArdCPZ(PIN_CPZ3);
  cpz4 = new ArdCPZ(PIN_CPZ4);
  cpz5 = new ArdCPZ(PIN_CPZ5);
  cpz6 = new ArdCPZ(PIN_CPZ6);
  cpz7 = new ArdCPZ(PIN_CPZ7);
  
}

void loop() 
{
bool ok1 = false;
bool ok2 = false;
bool ok3 = false;
bool ok4 = false;
bool ok5 = false;
bool ok6 = false;
bool ok7 = false;

  
  cpz1->check();

  if(cpz1->isBeaconConnecting())
  {
    byte addr[6];
    int ficha1 = 0;
    cpz1->getAddress(addr);
    Serial.print("\nBeacon-1: ");
    for(int i = 0; i < 6; i++) 
    {                               
      ficha1 = ficha1 + addr[i];    // Serial.print(addr[i], HEX);
    }
    Serial.print(" suma= ");
    Serial.println(ficha1);
    if (ficha1 == ficha1ok) ok1 = true;
    //    tone(sound,900,900);
  }

  cpz2->check();

  if(cpz2->isBeaconConnecting())
  {
    byte addr[6];
    int ficha2 = 0;
    cpz2->getAddress(addr);
    Serial.print("\nBeacon-2: ");
    for(int i = 0; i < 6; i++) 
    {                               
      ficha2 = ficha2 + addr[i];    // Serial.print(addr[i], HEX);
    }
    Serial.print(" suma= ");
    Serial.println(ficha2);
    if (ficha2 == ficha2ok) ok2 = true;
    //    tone(sound,900,900);
  }

  cpz3->check();

  if(cpz3->isBeaconConnecting())
  {
    byte addr[6];
    int ficha3 = 0;
    cpz3->getAddress(addr);
    Serial.print("\nBeacon-3: ");
    for(int i = 0; i < 6; i++) 
    {                               
      ficha3 = ficha3 + addr[i];    // Serial.print(addr[i], HEX);
    }
    Serial.print(" suma= ");
    Serial.println(ficha3);
    if (ficha3 == ficha3ok) ok3 = true;
    //    tone(sound,900,900);
  }

  cpz4->check();

  if(cpz4->isBeaconConnecting())
  {
    byte addr[6];
    int ficha4 = 0;
    cpz4->getAddress(addr);
    Serial.print("\nBeacon-4: ");
    for(int i = 0; i < 6; i++) 
    {                               
      ficha4 = ficha4 + addr[i];    // Serial.print(addr[i], HEX);
    }
    Serial.print(" suma= ");
    Serial.println(ficha4);
    if (ficha4 == ficha4ok) ok4 = true;
    //    tone(sound,900,900);
  }

  cpz5->check();

  if(cpz5->isBeaconConnecting())
  {
    byte addr[6];
    int ficha5 = 0;
    cpz5->getAddress(addr);
    Serial.print("\nBeacon-5: ");
    for(int i = 0; i < 6; i++) 
    {                               
      ficha5 = ficha5 + addr[i];    // Serial.print(addr[i], HEX);
    }
    Serial.print(" suma= ");
    Serial.println(ficha5);
    if (ficha5 == ficha5ok) ok5 = true;
    //    tone(sound,900,900);
  }

  cpz6->check();

  if(cpz6->isBeaconConnecting())
  {
    byte addr[6];
    int ficha6 = 0;
    cpz6->getAddress(addr);
    Serial.print("\nBeacon-6: ");
    for(int i = 0; i < 6; i++) 
    {                               
      ficha6 = ficha6 + addr[i];    // Serial.print(addr[i], HEX);
    }
    Serial.print(" suma= ");
    Serial.println(ficha6);
    if (ficha6 == ficha6ok) ok6 = true;
    //    tone(sound,900,900);
  }

  cpz7->check();

  if(cpz7->isBeaconConnecting())
  {
    byte addr[6];
    int ficha7 = 0;
    cpz7->getAddress(addr);
    Serial.print("\nBeacon-7: ");
    for(int i = 0; i < 6; i++) 
    {                               
      ficha7 = ficha7 + addr[i];    // Serial.print(addr[i], HEX);
    }
    Serial.print(" suma= ");
    Serial.println(ficha7);
    if (ficha7 == ficha7ok) ok7 = true;
    //    tone(sound,900,900);
  }


/*
   cpz2->check();

 if(cpz2->isBeaconConnecting())
  {
    byte addr[6];
    int ficha2 = 0;
    cpz2->getAddress(addr);

    Serial.print("\nBeacon-2: ");
    for(int i = 0; i < 6; i++)
    {
     // Serial.print(addr[i], HEX);
      ficha2 = ficha2 + addr[i];
    }
    Serial.print(" suma= ");
    Serial.println(ficha2);
    if (ficha2 == ficha2ok) ok2 = true;
    tone(sound,900,900);
   
  }
  else
  {
//    Serial.print("\n\n 2 Not found");
  }


cpz3->check();

 if(cpz3->isBeaconConnecting())
  {
    byte addr[6];
    int ficha3 = 0;
    cpz3->getAddress(addr);

    Serial.print("\nBeacon-3: ");
    for(int i = 0; i < 6; i++)
    {
     // Serial.print(addr[i], HEX);
      ficha3 = ficha3 + addr[i];
      tone(sound,900,900);
    }
    Serial.print(" suma= ");
    Serial.println(ficha3);
    if (ficha3 == ficha3ok) ok3 = true;
  
  }
  else
  {
//    Serial.print("\n\n 3 Not found");
  }


cpz4->check();

 if(cpz4->isBeaconConnecting())
  {
    byte addr[6];
    int ficha4 = 0;
    cpz4->getAddress(addr);

    Serial.print("\nBeacon-4: ");
    for(int i = 0; i < 6; i++)
    {
     // Serial.print(addr[i], HEX);
      ficha4 = ficha4 + addr[i];
      tone(sound,900,900);
    }
    Serial.print(" suma= ");
    Serial.println(ficha4);
    if (ficha4 == ficha4ok) ok4 = true;
   
  }
  else
  {
//    Serial.print("\n\n 4 Not found");
  }

*/

if (ok1 && ok2) {
  Serial.println(" \n \n Victory !!!" );
  delay(3000);
}
  
  delay(200);
//  Serial.println("\n******************************************* ");
}

