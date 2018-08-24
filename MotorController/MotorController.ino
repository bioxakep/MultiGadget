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

int mainDoor  = A1;
int underDoor = A7;

int poseiPump = A4;
int poseiValv = A5;
int poseiLock = A6;

int command = 0;
int thisI2CAddr = 21;
//int courtainUP = not implemented yet
//int courtainDN = not implemented yet

void setup() {
  Serial.begin(9600);
  Wire.begin(thisI2CAddr);

  pinMode(remote1, INPUT_PULLUP);
  pinMode(remote2, INPUT_PULLUP);
  pinMode(remote3, INPUT_PULLUP);
  pinMode(remote4, INPUT_PULLUP);

  pinMode(gateUP, OUTPUT);    // just close contact for 15 seconds to complete the action or until gateTOP is LOW

  pinMode(columnDN, OUTPUT);  // close contact for xx seconds or count columnOOO till 300
  pinMode(columnUP, OUTPUT);  // close contact for xx seconds or count columnOOO till 300

  pinMode(grapeUP,  OUTPUT);
  pinMode(grapeDN,  OUTPUT);

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

  if (digitalRead(remote1) == LOW) { // manual  control by radio remote control
    Serial.println("\nRemote control (1)");
    command = 0x02;
  }
  if (digitalRead(remote2) == LOW) { // manual  control by radio remote control
    Serial.println("\nRemote control (2)");
    command = 0x04;
  }


  if (command == 0x01) { // gate UP  - top sensor available
    Serial.println("\nOpen the gate start...");
    gateUp();
    command = 0;
    Serial.println("\nOpen the gate done");
  }

  if (command == 0x02) { // cloud up  - top sensor available
    Serial.println("\nCloud UP start...");
    cloudUp();
    command = 0;
    Serial.println("\nCloud UP done");
  }

  if (command == 0x03) { // cloud down  - no sensor available
    Serial.println("\nCloud DOWN start...");
    cloudDown();
    command = 0;
    motor = 0;
    Serial.println("\nCloud DOWN done");
  }

  if (command == 0x04) { // grape down  - down sensor available
    Serial.println("\nGrape DOWN start...");
    grapeDown();
    command = 0;
    motor = 0;
    Serial.println("\nGrape DOWN done");
  }

  if (command == 0x05) { // grape up  - top sensor available
    Serial.println("\nGrape UP start...");
    grapeUp();
    command = 0;
    motor = 0;
    Serial.println("\nGrape UP done");
  }

  if (command == 0x06) { // column down  - down sensor available
    Serial.println("\nColumn DOWN start...");
    columnDown();
    command = 0;
    motor = 0;
    Serial.println("\nColumn DOWN done");
  }

  if (command == 0x07) { // column up  - top sensor available
    Serial.println("\nColumn UP start...");
    columnUp();
    command = 0;
    motor = 0;
    Serial.println("\nColumn UP done");
  }

  if (command == 0x08) { // open underground door
    Serial.println("\nunderground Door OPEN");
    digitalWrite(underDoor, LOW);
    command = 0;
  }

  if (command == 0x09) { // open main door
    Serial.println("\nmain Door OPEN");
    digitalWrite(mainDoor, LOW);
    command = 0;
  }

  if (command == 0xFF) { // poseidon - lets open poseidon vault
    Serial.println("\nPoseidon vault OPENING...");
    poseiVaultOpen();
    Serial.println("\nPoseidon vault OPEN");
    command = 0;
  }
}




void checkInputs()
{
  Serial.print("remote1   = ");
  Serial.println(digitalRead(remote1) ? "HIGH" : "LOW");
  Serial.print("remote2   = ");
  Serial.println(digitalRead(remote2) ? "HIGH" : "LOW");
  Serial.print("remote3   = ");
  Serial.println(digitalRead(remote3) ? "HIGH" : "LOW");
  Serial.print("remote4   = ");
  Serial.println(digitalRead(remote4) ? "HIGH" : "LOW");
  Serial.print("\ngateTOP   = ");
  Serial.println(digitalRead(gateTOP) ? "HIGH" : "LOW");
  Serial.print("columnTOP = ");
  Serial.println(digitalRead(columnTOP) ? "HIGH" : "LOW");
  Serial.print("columnBOT = ");
  Serial.println(digitalRead(columnBOT) ? "HIGH" : "LOW");
  Serial.print("grapeTOP  = ");
  Serial.println(digitalRead(grapeTOP) ? "HIGH" : "LOW");
  Serial.print("grapeBOT  = ");
  Serial.println(digitalRead(grapeBOT) ? "HIGH" : "LOW");
  Serial.print("cloudTOP  = ");
  Serial.println(digitalRead(cloudTOP) ? "HIGH" : "LOW");

}

