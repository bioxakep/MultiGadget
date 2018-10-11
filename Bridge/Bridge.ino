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
  if (!monitorConnected) Serial.print("Monitor connecting...");
  //connectToMonitor();
  if (!monitorConnected) Serial.println("OK");
}

void loop()
{
  unsigned long tick = millis();
  if (tick - masterLastRecTime > masterTimeOut && masterConnected)
  {
    masterConnected = false;
    Serial.println("masterLastRecTime:" + String(masterLastRecTime));
    Serial.println("tick:" + String(tick));
    Serial.println("masterTimeOut:" + String(masterTimeOut));
    Serial.println("MASTER DISCONNECTED");
  }
  if (!masterConnected)
  {
    unsigned long startConnect = tick;
    unsigned long whileTick = tick;
    byte inByte = 0;
    int connectWaitCount = 0;
    if (!monitorConnected) Serial.print("Connecting to master...");
    int connCount = 0;
    while (!masterConnected && (whileTick - startConnect < masterConnTimeOut))
    {
      while (!masterSerial.available()) {
        whileTick = millis();
        if(whileTick - startConnect + connectWaitCount*1000 > 1000 && inByte == 0)
        {
          connectWaitCount++;
          if (!monitorConnected) Serial.print(String(connectWaitCount) + "...");
        }
      }
      inByte = masterSerial.read();
      delay(10);
      if (inByte == 0xA3) masterConnected = true;
      if (inByte == 0xA1 || inByte == 0xA2 || inByte == 0xA3)
      {
        if (!monitorConnected) Serial.println("/nConnecting... recieved and sending back: "+String(inByte)+ " in " + String(whileTick));
        digitalWrite(serialTXControl, HIGH);  // Init Transmitter
        masterSerial.write(inByte);
        delay(10);
        digitalWrite(serialTXControl, LOW);  // Stop Transmitter
      }
    }
    if (!masterConnected) 
    {
      if (!monitorConnected) Serial.println("MASTER DISCONNECTED LONG TIME");
      else Serial.println("MDLT");
    }
    else Serial.println("/nMASTER CONNECTED/n");
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
          if (input[i] == 0x05) passGStates[i] = true;
          else passGStates[i] = false;
        }
        byte last = Serial.read();
        if (last == 0xFF) //All Data Recieved marker
        {
          // Prepare to send states to Master
          digitalWrite(serialTXControl, HIGH);  // Init Transmitter
          masterSerial.write(0xBB);
          delay(10);
          //Sending...
          for (int d = 0; d < 31; d++)
          {
            masterSerial.write(passGStates[d]);
            delay(10);
          }
          masterSerial.write(0xFF); // End sending
          delay(10);
          digitalWrite(serialTXControl, LOW);  // Stop Transmitter
        }
      }
      else Serial.flush();
    }

    if (masterSerial.available() > 0) //recieve from master
    {
      byte input[31];
      for (byte i = 0; i < 31; i++) input[i] = 0x00;
      byte inByte = masterSerial.read();
      //masterSerial.println("First byte is " + String(inByte));
      if (inByte == 0xA5)
      {
        for (int s = 0; s < 31; s++) passGStates[s] = false;
        if (!monitorConnected) Serial.println("Send to operator clear states");
        Serial.write(0xBB);
        //Sending...
        for (int d = 0; d < 31; d++)
        {
          Serial.print(passGStates[d]);
        }
        Serial.write(0xFF);
      }
      else if (inByte == 0xAA)
      {
        masterLastRecTime = tick;
        if (!monitorConnected) Serial.print("Recieving data from master: ");
        delay(5);
        for (int i = 0; i < 31; i++)
        {
          input[i] = masterSerial.read();
          delay(5);
          Serial.print("|" + String(input[i]));
          if (input[i] > 0x03) passGStates[i] = true;
          else passGStates[i] = false;
        }
        if (!monitorConnected) Serial.println();
        byte last = masterSerial.read();
        Serial.println("Last byte is " + String(last));
        if (last == 0xFF)
        {
          if (!monitorConnected) Serial.print("Send to operator: ");
          // Prepare to send states to Operator
          Serial.write(0xBB);
          //Sending...
          for (int d = 0; d < 31; d++)
          {
            Serial.print(passGStates[d]);
            delay(2);
          }
          Serial.write(last);
          if (!monitorConnected) Serial.println(); // for test only
        }
      }
      while (masterSerial.available()) masterSerial.read();
    }
  }
}
