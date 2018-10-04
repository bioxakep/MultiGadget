#include <SoftwareSerial.h>
//0xAA from Master
//0xBB from Bridge
//0xCC from operator
SoftwareSerial masterSerial(10, 11);
int serialTXControl = 3;
boolean operGStates[31];
boolean passGStates[31];

void setup() {
  masterSerial.begin(9600);
  Serial.begin(9600);
  pinMode(serialTXControl, OUTPUT);
  digitalWrite(serialTXControl, LOW);
  for (int s = 0; s < 31; s++)
  {
    operGStates[s] = false;
    passGStates[s] = false;
  }
  //connectToMonitor();
  Serial.print("Master connecting...");
  connectToMaster();
  Serial.println("Bridge started");
}

void loop() {
  if (Serial.available() > 0) //recieve from operator processing
  {
    byte input[31];
    byte inByte = Serial.read();
    if (inByte == 0xCC)
    {
      for (int i = 0; i < 31; i++)
      {
        input[i] = Serial.read();
        if (input[i]) operGStates[i] = true;
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
          masterSerial.write(operGStates[d]);
          delay(2);
        }
        masterSerial.write(0xFF);
        digitalWrite(serialTXControl, LOW);  // Stop Transmitter
        // End sending...
      }
    }
    else Serial.flush();
  }

  if (masterSerial.available() > 0) //recieve from master
  {
    //Serial.println("Info from Master recieved");
    byte input[31];
    byte inByte = masterSerial.read();
    if (inByte == 0xAA)
    {
      for (int i = 0; i < 31; i++)
      {
        input[i] = masterSerial.read();
        if (input[i]) operGStates[i] = true;
      }
      byte last = masterSerial.read();
      if (last == 0xFF)
      {
        Serial.println("send to oper");
        // Prepare to send states to Operator
        Serial.write(0xBB);
        //Sending...
        for (int d = 0; d < 31; d++)
        {
          Serial.print(operGStates[d]);
          delay(2);
        }
        Serial.write(0xFF);
        // End sending...
      }
    }
    else 
    {
      masterSerial.flush();
    }
  }
}
