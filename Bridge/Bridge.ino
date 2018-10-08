/*
   Передумал всю связь.
  Т.к. мост  и оператор чаще всего включены и ждут данные от мастера и оператора, то:
  1. Мастер и мост.
  Мост при запуске подключается к монитору и сразу попадает в loop, где:
  Если от мастера нет данных более 10 секунд то переходим в режим подключения к нему.
  Если данные идут то проверяем их правильность и передаём монитору.
  В режиме подключения ждем сигнального сообщения от мастера после которого
  прекращаем отправлять в цикле своё сигнальное сообщение и переходим в режим передачи данных.
*/
// 08.10.2018 rewrote connection
#include <SoftwareSerial.h>
//0xAA from Master
//0xBB from Bridge
//0xCC from operator
SoftwareSerial masterSerial(10, 11);
int serialTXControl = 3;
boolean passGStates[31];
boolean monitorConnected = false;
boolean masterConnected = false;
unsigned long mConnectTime = 0;
unsigned long mDisconnectTime = 0;
unsigned long masterLastRecTime = 0;
unsigned long masterTimeOut = 5000;
unsigned long masterConnTimeOut = 20000;
void setup() {
  masterSerial.begin(9600);
  Serial.begin(9600);
  pinMode(serialTXControl, OUTPUT);
  digitalWrite(serialTXControl, LOW);
  for (int s = 0; s < 31; s++) passGStates[s] = false;
  if(!monitorConnected) Serial.print("Monitor connecting...");
  //connectToMonitor();
  if(!monitorConnected) Serial.println("OK");
}

void loop() 
{
  unsigned long tick = millis();
  if (!masterConnected)
  {
    unsigned long startConnect = tick;
    unsigned long whileTick = tick;
    if(!monitorConnected) Serial.print("Connecting to master...");
    int connCount = 0;
    while (!masterConnected && (whileTick - startConnect < masterConnTimeOut))
    {
      whileTick = millis();
      digitalWrite(serialTXControl, HIGH);  // Init Transmitter
      masterSerial.write(0xC2);
      digitalWrite(serialTXControl, LOW);  // Init Transmitter
      connCount++;
      if(!monitorConnected) Serial.print(String(connCount)+"..");
      while (millis() - whileTick < 500)
      {
        if (masterSerial.available() > 0)
        {
          byte in = masterSerial.read();
          if (in == 0xC1) masterConnected = true;
        }
      }
    }
    if(!masterConnected) Serial.println("MASTER DISCONNECTED LONG TIME");
    else Serial.println("OK");
  }
  else // Master Connected
  {
    if (Serial.available() > 0) //recieve from operator processing
    {
      byte input[31];
      byte inByte = Serial.read();
      if (inByte == 0xCC) // Start-signal of DATA recieve from Operator
      {
        for (int i = 0; i < 31; i++)
        {
          input[i] = Serial.read();
          if (input[i] == 0x01) passGStates[i] = true;
          else passGStates[i] = false;
          delay(5);
        }
        byte last = Serial.read();
        if (last == 0xFF) //All Data Recieved marker
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
          masterSerial.write(0xFF); // End sending
          digitalWrite(serialTXControl, LOW);  // Stop Transmitter
        }
      }
      else Serial.flush();
    }
    
    if (masterSerial.available() > 0) //recieve from master
    {
      byte input[31];
      byte inByte = masterSerial.read();
      if(inByte == 0xC1) //Master starts when bridge sync-ed and wait data...
      {
        if(!monitorConnected) Serial.println("Master Restarted and try sync again");
        digitalWrite(serialTXControl, HIGH);  // Init Transmitter
        masterSerial.write(0xC3);
        digitalWrite(serialTXControl, LOW);  // Stop Transmitter
        boolean masterSync = false;
        while(!masterSync)
        {
          if(masterSerial.available() > 0)
          {
            if(masterSerial.read() == 0xAA)
            {
              inByte = 0xAA;
              masterSync = true;
            }
          }
        }
        if(!monitorConnected) Serial.println("Master Connected, reading data...");
      }
      if (inByte == 0xC4)
      {
        for (int s = 0; s < 31; s++) passGStates[s] = false;
        if(!monitorConnected) Serial.println("Send to operator clear states");
        Serial.write(0xBB);
          //Sending...
          for (int d = 0; d < 31; d++)
          {
            Serial.print(passGStates[d]);
            delay(2);
          }
          Serial.write(0xFF);
      }
      if (inByte == 0xAA)
      {
        masterLastRecTime = millis();
        if(!monitorConnected) Serial.print("Recieving data from master: ");
        for (int i = 0; i < 31; i++)
        {
          input[i] = masterSerial.read();
          delay(5);
          if(!monitorConnected) Serial.print(input[i]);
          if (input[i] > 0x03) passGStates[i] = true;
          else passGStates[i] = false;
        }
        if(!monitorConnected) Serial.println();
        byte last = masterSerial.read();
        if (last == 0xFF)
        {
          if(!monitorConnected) Serial.print("Send to operator: ");
          // Prepare to send states to Operator
          Serial.write(0xBB);
          //Sending...
          for (int d = 0; d < 31; d++)
          {
            Serial.print(passGStates[d]);
            delay(2);
          }
          Serial.write(0xFF);
          if(!monitorConnected) Serial.println(); // for test only
        }
      }
      else
      {
        masterSerial.flush();
      }
    }
    if(tick - masterLastRecTime > masterTimeOut) 
    {
      masterConnected = false;
      Serial.println("MASTER DISCONNECTED");
    }
  }
}
