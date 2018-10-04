#include <SoftwareSerial.h>
//0xAA from Master
//0xBB from Bridge
//0xCC from operator
SoftwareSerial masterSerial(10, 11);
int serialTXControl = 3;
boolean passGStates[31];

void setup() {
  masterSerial.begin(9600);
  Serial.begin(9600);
  pinMode(serialTXControl, OUTPUT);
  digitalWrite(serialTXControl, LOW);
  for (int s = 0; s < 31; s++) passGStates[s] = false;
  //connectToMonitor();
  Serial.print("Master connecting...");
  connectToMaster();
  Serial.println("Bridge started");
}

void loop() {
  /*if (Serial.available() > 0) //recieve from operator processing
  {
    byte input[31];
    byte inByte = Serial.read();
    if (inByte == 0xCC)
    {
      for (int i = 0; i < 31; i++)
      {
        input[i] = Serial.read();
        if (input[i] == 0x01) passGStates[i] = true;
        else passGStates[i] = false;
        delay(5);
      }
      byte last = Serial.read();
      if (last == 0xFF) //All Data Recieved
      {
        // Prepare to send states to Master
        digitalWrite(serialTXControl, HIGH);  // Init Transmitter
        masterSerial.write(0xBB);
        //Sending...
        for (int d = 0; d < 31; d++)
        {
          masterSerial.write(passGStates[d]);
          delay(2);
        }
        masterSerial.write(0xFF);
        digitalWrite(serialTXControl, LOW);  // Stop Transmitter
        // End sending...
      }
    }
    else Serial.flush();
  }
  */
  if (masterSerial.available() > 0) //recieve from master
  {
    //Serial.println("Info from Master recieved");
    byte input[31];
    byte inByte = masterSerial.read();
    if (inByte == 0xAA)
    {
      Serial.print("Recieving data from master: ");
      for (int i = 0; i < 31; i++)
      {
        input[i] = masterSerial.read();
        Serial.print(input[i], BIN);
        if (input[i] == 0x01) passGStates[i] = true;
        else passGStates[i] = false;
      }
      Serial.println();
      byte last = masterSerial.read();
      if (last == 0xFF)
      {
        Serial.print("Send data to oper: ");
        // Prepare to send states to Operator
        Serial.write(0xBB);
        //Sending...
        for (int d = 0; d < 31; d++)
        {
          Serial.print(passGStates[d]);
          delay(2);
        }
        Serial.write(0xFF);
        Serial.println();
      }
    }
    else 
    {
      masterSerial.flush();
    }
  }
}
