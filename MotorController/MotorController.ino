// Motor Comtroller v1
// 13 AGO 2018
// 21 AGO 2018 adjusted - I2C enabled - xBee deleted

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

//int courtainUP = not implemented yet
//int courtainDN = not implemented yet 

void setup() {
Serial.begin(9600);

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
digitalWrite(underDoor,LOW);

digitalWrite(poseivalve,LOW);    // unpowered valve dont let water get out of the vault
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
 digitalWrite(columnUP,LOW);
 digitalWrite(columnDN,LOW);
 digitalWrite(grapeUP, LOW);
 digitalWrite(grapeDN, LOW);
 digitalWrite(cloudUP, LOW);
 digitalWrite(cloudDN, LOW);

// receives command from master via i2c

  if (digitalRead(remote1) == LOW) { // manual  control by radio remote control
    Serial.println("\nRemote control (1)");
    command = 2222;
   }
  if (digitalRead(remote2) == LOW) { // manual  control by radio remote control
    Serial.println("\nRemote control (2)");
    command = 4444;
   }
  
  
  if (command == 1111) { // gate UP  - top sensor available
    Serial.println("\nOpen the gate start...");
    while(digitalRead(gateTOP) == HIGH) {
     digitalWrite(gateUP,HIGH);
    }
    digitalWrite(gateUP,LOW);
    command = 0;
    Serial.println("\nOpen the gate done");
  }

  if (command == 2222) { // cloud up  - top sensor available
    Serial.println("\nCloud UP start...");
    while(digitalRead(cloudTOP) == HIGH) {
     digitalWrite(cloudUP,HIGH);
    }
    digitalWrite(cloudUP,LOW);
    command = 0;
    Serial.println("\nCloud UP done");
  }

  if (command == 3333) { // cloud down  - no sensor available
    Serial.println("\nCloud DOWN start...");
    int motor = 0;
    while (motor < 50) {   // revolutions of motor
    digitalWrite(cloudDN, HIGH);
     if (digitalRead(cloudOOO) == LOW) {
      motor++;
      while(digitalRead(cloudOOO)== LOW) { ; }
     }
    }
     digitalWrite(cloudDN,LOW);
     command = 0;
     motor = 0;
    Serial.println("\nCloud DOWN done");
  }

  if (command == 4444) { // grape down  - down sensor available
   Serial.println("\nGrape DOWN start...");
   int motor = 0;
    while (motor < 20 || digitalRead(grapeBOT) == HIGH ) {   // revolutions of motor
    digitalWrite(grapeDN, HIGH);
     if (digitalRead(grapeOOO) == LOW) {
      motor++;
      while(digitalRead(grapeOOO)== LOW) { ; }
     }
    }
     digitalWrite(grapeDN,LOW);
     command = 0;
     motor = 0;
    Serial.println("\nGrape DOWN done");
  }

  if (command == 5555) { // grape up  - top sensor available
    Serial.println("\nGrape UP start...");
    int motor = 0;
    while (motor < 20 || digitalRead(grapeTOP) == HIGH ) {   // revolutions of motor OR top sensor
    digitalWrite(grapeUP, HIGH);
     if (digitalRead(grapeOOO) == LOW) {
      motor++;
      while(digitalRead(grapeOOO)== LOW) { ; }
     }
    }
     digitalWrite(grapeUP,LOW);
     command = 0;
     motor = 0;
    Serial.println("\nGrape UP done");
  }

  if (command == 6666) { // column down  - down sensor available
    Serial.println("\nColumn DOWN start...");
    int motor = 0;
    while (motor < 120 || digitalRead(columnBOT) == HIGH ) {   // revolutions of motor
    digitalWrite(columnDN, HIGH);
     if (digitalRead(columnOOO) == LOW) {
      motor++;
      while(digitalRead(columnOOO) == LOW) { ; }
     }
    }
     digitalWrite(columnDN,LOW);
     command = 0;
     motor = 0;
    Serial.println("\nColumn DOWN done");
  }

  if (command == 7777) { // column up  - top sensor available
    Serial.println("\nColumn UP start...");
    int motor = 0;
    while (motor < 120 || digitalRead(columnTOP) == HIGH ) {   // revolutions of motor OR top sensor
    digitalWrite(columnUP, HIGH);
     if (digitalRead(columnOOO) == LOW) {
      motor++;
      while(digitalRead(columnOOO) == LOW) { ; }
     }
    }
     digitalWrite(columnUP,LOW);
     command = 0;
     motor = 0;
    Serial.println("\nColumn UP done");
  }

  if (command == 8888) { // open underground door
   Serial.println("\nunderground Door OPEN");
   digitalWrite(underDoor, LOW);
   command = 0;
  }

  if (command == 9999) { // open main door
   Serial.println("\nmain Door OPEN");
   digitalWrite(mainDoor, LOW);
   command = 0;
  }

  if (command == 1234) { // poseidon - lets open poseidon vault
   Serial.println("\nPoseidon vault OPENING...");
   digitalWrite(poseiValv, HIGH);    //drain the water
   delay(5000);
   digitalWrite(poseiLock, LOW);     // water drained, open the top cover
   digitalWrite(poseiValv, LOW);   
   Serial.println("\nPoseidon vault OPEN");
   command = 0;
  }


  
}  




void checkInputs() {
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

