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
unsigned long mLastA9Rec = 0;
unsigned long masterConnTimeOut = 20000;
void setup() {
  masterSerial.begin(9600);
  Serial.begin(9600);
  pinMode(serialTXControl, OUTPUT);
  digitalWrite(serialTXControl, LOW);
  for (int s = 0; s < 31; s++) passGStates[s] = false;
  connectToMonitor();
}

void loop()
{
  unsigned long tick = millis();

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
        if (whileTick - (startConnect + connectWaitCount * 1000) > 1000 && inByte == 0)
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
        if (!monitorConnected) Serial.println("\nConnecting... recieved and sending back: " + String(inByte) + " in " + String(whileTick));
        digitalWrite(serialTXControl, HIGH);  // Init Transmitter
        masterSerial.write(inByte);
        delay(10);
        digitalWrite(serialTXControl, LOW);  // Stop Transmitter
      }
      if (inByte == 0xA9)
      {
        delay(350);
        while (masterSerial.available()) masterSerial.read();
        digitalWrite(serialTXControl, HIGH);  // Init Transmitter
        digitalWrite(13, HIGH);
        masterSerial.write(0xBA);
        delay(10);
        digitalWrite(serialTXControl, LOW);  // Stop Transmitter
        digitalWrite(13, LOW);
      }
    }
    if (!masterConnected)
    {
      if (!monitorConnected) Serial.println("MASTER DISCONNECTED LONG TIME");
      else Serial.println("MDLT");
    }
    else Serial.println("\nRestart");
  }
  else // Master Connected
  {
    if (Serial.available() > 0) //recieve from operator processing
    {
      String input = Serial.readStringUntil('\n');
      if (input.startsWith("CC")) // Start-signal of DATA recieve from Operator
      {
        for (int i = 0; i < 31; i++)
        {
          if (input[i+2] == '5')
          {
            passGStates[i] = true;
            digitalWrite(13, HIGH);
            delay(10);
            digitalWrite(13, LOW);
          }
          else passGStates[i] = false;
        }
        if (input.endsWith("FF")) //All Data Recieved marker
        {
          digitalWrite(13,HIGH);
          // Prepare to send states to Master
          digitalWrite(serialTXControl, HIGH);  // Init Transmitter
          masterSerial.write(0xBB);
          delay(10);
          //Sending...
          for (int d = 0; d < 31; d++)
          {
            if (passGStates[d]) masterSerial.write(0x05);
            else masterSerial.write(0x01);
            delay(10);
          }
          masterSerial.write(0xFF); // End sending
          delay(10);
          digitalWrite(serialTXControl, LOW);  // Stop Transmitter
          digitalWrite(13, LOW);
        }
      }
      else if (input.startsWith("ClearStates"))
      {
        digitalWrite(13, HIGH);
        // Prepare to send states to Master
        digitalWrite(serialTXControl, HIGH);  // Init Transmitter
        masterSerial.write(0xCF);
        delay(10);
        digitalWrite(serialTXControl, LOW);  // Stop Transmitter
        digitalWrite(13, LOW);
      }
      else Serial.flush();
    }

    if (masterSerial.available() > 0) //recieve from master
    {
      byte input[31];
      for (byte i = 0; i < 31; i++) input[i] = 0x00;
      byte inByte = masterSerial.read();

      //masterSerial.println("First byte is " + String(inByte));
      if(inByte == 0xA1)
      {
        masterConnected = false;
        for (int s = 0; s < 31; s++) passGStates[s] = false;
      }
      else if (inByte == 0xA9)
      {
        mLastA9Rec = tick;
      }
      else if (inByte == 0xA5)
      {
        for (int s = 0; s < 31; s++) passGStates[s] = false;
        if (!monitorConnected) Serial.println("Send to operator clear states and run");
        Serial.write("masterStart\n");
        //Sending...
      }
      else if (inByte == 0xAA)
      {
        delay(350);
        if (!monitorConnected) Serial.print("Recieving data from master: ");
        for (int i = 0; i < 31; i++)
        {
          input[i] = masterSerial.read();
          //Serial.print("|" + String(input[i]));
          if (input[i] > 0x03) passGStates[i] = true;
          else passGStates[i] = false;
        }
        if (!monitorConnected) Serial.println();
        byte last = masterSerial.read();
        //Serial.println("Last byte is " + String(last));
        if (last == 0xFF)
        {
          if (!monitorConnected) Serial.print("Send to operator: ");

          // Prepare to send states to Operator
          String toOperator="BB";
          Serial.write("BB");
          for (int d = 0; d < 31; d++)
          {
            if (passGStates[d]) Serial.write("5");
            else Serial.write("1");
          }
          Serial.write("FF\n");
          if (!monitorConnected) Serial.println(); // for test only
        }
      }
      while (masterSerial.available()) masterSerial.read();
      if(mLastA9Rec - tick > 30000)
      {
        digitalWrite(serialTXControl, HIGH);  // Init Transmitter
        masterSerial.write(0xBA);
        delay(10);
        digitalWrite(serialTXControl, LOW);  // Stop Transmitter
      }
    }
  }
}
