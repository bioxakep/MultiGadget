// centralController 0.1
#include <SoftwareSerial.h>
#define SSerialRX 10  //Serial Receive pin
#define SSerialTX 11  //Serial Transmit pin
#define SSerialTxControl 3   //RS485 Direction control
#define RS485Transmit HIGH
#define RS485Receive LOW

SoftwareSerial rs485(SSerialRX, SSerialTX); // RX, TX
boolean devStates[] = {false, false, false, false, false, false, false, false, false, false, false, false,};
boolean prevDevStates[] = {false, false, false, false, false, false, false, false, false, false, false, false,};
int inputStates[] = {false, false, false, false, false, false, false, false, false, false, false, false,};
int currStates[] = {false, false, false, false, false, false, false, false, false, false, false, false,};
int devCount = 12;
void setup()
{
  Serial.begin(9600);
  pinMode(SSerialTxControl, OUTPUT);
  digitalWrite(SSerialTxControl, RS485Receive);  // Init Transceiver
  rs485.begin(9600);
  Serial.println("CentralController Started");
  connectToMonitor();
  randomSeed(A0);
}

void loop()
{
  //main loop
  /*
     1. Check RS485
        if change send to processing
        else idle
     2. Check the processing comands
        if comands available send to multiController
        else idle
  */
  //if(checkRS485()) sendToOperator();
  //if(checkCommands()) sendToMulti();


  String sendStr = "";
  for (int i = 0; i < devCount; i++)
  {
    devStates[i] = random(0,4) > 2;
    sendStr += String(devStates[i] == true);
  }
  Serial.println(sendStr);
  delay(3000);
}

void sendToMulti()
{
  byte sendByte[2] = {0x00, 0x00};
  digitalWrite(SSerialTxControl, RS485Transmit);  // Init Transmitter
  for (int i = 0; i < devCount; i++)
  {
    sendByte[i / 8] |= (devStates[i] << (i % 8));
  }
  rs485.write(sendByte[0]);
  delay(10);
  rs485.write(sendByte[1]);
  delay(10);
  digitalWrite(SSerialTxControl, RS485Receive);
}

boolean checkCommands()
{
  if (Serial.available() > 0)
  {
    String inputStr = Serial.readStringUntil('\n');
    if (inputStr.length() == devCount)
    {
      for (int i = 0; i < devCount; i++)
      {
        inputStates[i] = inputStr[i] > 0;
        prevDevStates[i] = devStates[i];
        devStates[i] = inputStates[i];
      }
    }
    return true;
  }
  else return false;
}

boolean checkRS485()
{
  if (rs485.available())
  {
    byte inByte[2] = {0x00, 0x00};
    inByte[0] = rs485.read();
    inByte[1] = rs485.read();
    for (int i = 0; i < devCount; i++)
    {
      inputStates[i] = (inByte[i / 8] >> (i % 8)) & 0x01;
    }
    int setSum = 0;
    for (int j = 5; j < 8; j++)
    {
      if (((inByte[1] >> j) & 1) == 1) setSum++;
    }
    if (setSum == 4) return true;
  }
  return false;
}

void sendToOperator()
{
  String sendStr = "";
  for (int i = 0; i < devCount; i++)
  {
    prevDevStates[i] = devStates[i];
    devStates[i] = inputStates[i];
    sendStr += String(devStates[i] == true);
  }
  if (sendStr.length() == devCount) Serial.println(sendStr);
}

void connectToMonitor()
{
  boolean monConnected = false;
  while (!monConnected)
  {
    if (Serial.available() > 0)
    {
      String input = "";
      input = Serial.readStringUntil('\n');
      if (input.startsWith("letsgame"))
      {
        Serial.println("start");
        delay(500);
        digitalWrite(13, HIGH);
        boolean sync = false;
        while (!sync)
        {
          if (Serial.available() > 0)
          {
            Serial.readStringUntil('\n');
            delay(50);
            Serial.println("start");
            delay(500);
          }
          else sync = true;
        }
        monConnected = true;
      }
    }
  }
}

