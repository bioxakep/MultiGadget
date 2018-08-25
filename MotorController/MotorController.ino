// Motor Comtroller v1
// 13 AGO 2018
// 21 AGO 2018 adjusted - I2C enabled - xBee deleted
#include "Wire.h" // I2C


int remote1   = 31;
int remote2   = 35;
int remote3   = 39;
int remote4   = 43;

int gateUP    = A2;
int gateTOP   = A11;

int columnDN  = A12;
int columnUP  = A13;
int columnTOP = A8;
int columnBOT = A9;
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

void setup() {
  Serial.begin(9600);
  Wire.begin(thisI2CAddr);
  Wire.onReceive(receiveEvent);
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

  digitalWrite(poseivalve, LOW);   // unpowered valve dont let water get out of the vault
  digitalWrite(poseiPump, HIGH);
  delay(10000);                    /// some time to fill the poseidon vault with water
  digitalWrite(poseiPump, Low);
  digitalWrite(poseiLock, HIGH);   // lock the cover of the vault while if full of water

  Serial.println("\nMotor Controller v1  \n21_AGO_2018 ");
  Serial.println("Hardware = Mega\n");
  checkInputs();
}

void loop() {

  digitalWrite(gateUP,  LOW);
  digitalWrite(columnUP, LOW);
  digitalWrite(columnDN, LOW);
  digitalWrite(grapeUP, LOW);
  digitalWrite(grapeDN, LOW);
  digitalWrite(cloudUP, LOW);
  digitalWrite(cloudDN, LOW);

  // receives command from master via i2c

  if (!digitalRead(remote1)) { // manual  control by radio remote control
    Serial.println("\nRemote control (1)");
    command = 0x02;
    //cloudUp();
  }
  if (!digitalRead(remote2)) { // manual  control by radio remote control
    Serial.println("\nRemote control (2)");
    command = 0x04;
    //grapeDown();
  }


  if (command == 0x01) { // gate UP  - top sensor available
    Serial.print("\nOpen the gate start...");
    cloudUp();
    grapeUp();
    columnUp();
    Serial.println("Done.");
  }

  if (command == 0x03) { // cloud down  - no sensor available
    Serial.print("\ncurtainUP start...");
    digitalWrite(ncurtainUP, HIGH);
    delay(200);
    digitalWrite(ncurtainUP, LOW);
    Serial.println("Done.");
  }

  if (command == 0x04) { // grape down  - down sensor available
    Serial.print("\nGate UP start...");
    gateUp();
    Serial.println("Done.");
  }
  /*
  if (command == 0x05) { // grape up  - top sensor available
    Serial.print("\nGrape UP start...");
    grapeUp();
    
    Serial.println("Done.");
  }

  if (command == 0x06) { // column down  - down sensor available
    Serial.println("\nColumn DOWN start...");
    columnDown();
    
    Serial.println("\nColumn DOWN done");
  }

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
  motor = 0;
}

void receiveEvent(int howMany) 
{
  if (Wire.available()) // пройтись по всем до последнего
  { 
    byte c = Wire.read();    // принять байт как символ
    if(c > 0x00 && c < 0x20) command = c;
  }
}

