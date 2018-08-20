// multiController 0.1
#include <SoftwareSerial.h>

#define SSerialRX 10  //Serial Receive pin
#define SSerialTX 11  //Serial Transmit pin
#define SSerialTxControl 3   //RS485 Direction control
#define RS485Transmit HIGH
#define RS485Receive LOW
#define devCount 12
SoftwareSerial rs485(SSerialRX, SSerialTX); // RX, TX

boolean devStates[] = {false, false, false, false, false, false, false, false, false, false, false, false,};
boolean prevDevStates[] = {false, false, false, false, false, false, false, false, false, false, false, false,};
int inputStates[] = {false, false, false, false, false, false, false, false, false, false, false, false,};
int currStates[] = {false, false, false, false, false, false, false, false, false, false, false, false,};
int deviceSetPins[] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33};
int deviceGetPins[] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33};


void setup()
{
  pinMode(SSerialTxControl, OUTPUT);
  digitalWrite(SSerialTxControl, RS485Receive);  // Init Transceiver
  for (int d = 0; d < devCount; d++)
  {
    pinMode(deviceSetPins[d], OUTPUT);
    pinMode(deviceGetPins[d], INPUT_PULLUP);
    digitalWrite(deviceSetPins[d], LOW);
    Serial.println("Dev " + String(d + 1) + " is" + String(digitalRead(deviceGetPins[d]), BIN));
  }
  rs485.begin(9600);
  Serial.println("multiController Started");
}

void loop()
{
  if (checkRS485()) setDevices();
  if (checkDevices()) sendStates();
}
void sendStates()
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

boolean checkDevices()
{
  boolean changed = false;
  for (int d = 0; d < devCount; d++)
  {
    currStates[d] = digitalRead(deviceGetPins[d]);
    if (currStates[d] != devStates[d])
    {
      changed = true;
      prevDevStates[d] = devStates[d];
      devStates[d] = currStates[d];
    }
  }
}

void setDevices()
{
  for (int d = 0; d < devCount; d++)
  {
    prevDevStates[d] = devStates[d];
    devStates[d] = inputStates[d];
  }
}

