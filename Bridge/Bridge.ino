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
// 22.10.2018 rewrote connection
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
int gadgetCount = 31;
void setup() {
  masterSerial.begin(9600);
  Serial.begin(9600);
  pinMode(serialTXControl, OUTPUT);
  digitalWrite(serialTXControl, LOW);
  for (int s = 0; s < gadgetCount; s++) passGStates[s] = false;
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
    int connCount = 0;
    while (!masterConnected && (whileTick - startConnect < masterConnTimeOut))
    {
      whileTick = millis();
      if (whileTick - (startConnect + connectWaitCount * 1000) > 1000 && inByte == 0)
      {
        connectWaitCount++;
        if (!monitorConnected) Serial.print(String(connectWaitCount) + "...");
      }
      if (masterSerial.available())
      {
        inByte = masterSerial.read();
        delay(10);
        if (inByte == 0xA9)
        {
          delay(350);
          while (masterSerial.available()) masterSerial.read();
          digitalWrite(serialTXControl, HIGH);  // Init Transmitter
          digitalWrite(13, HIGH);
          masterSerial.write(0xBC);
          delay(10);
          digitalWrite(serialTXControl, LOW);  // Stop Transmitter
          digitalWrite(13, LOW);
          Serial.println("Send resync to master");
        }
        else if (inByte == 0xA1 || inByte == 0xA2 || inByte == 0xA3)
        {
          if (!monitorConnected) Serial.println("\nConnecting... recieved and sending back: " + String(inByte) + " in " + String(whileTick));
          digitalWrite(serialTXControl, HIGH);  // Init Transmitter
          masterSerial.write(inByte);
          delay(10);
          digitalWrite(serialTXControl, LOW);  // Stop Transmitter
        }
        if (inByte == 0xA3)
        {
          masterConnected = true;
          mLastA9Rec = whileTick;
          Serial.println("masterConnected");
        }
      }
    }
    if (!masterConnected)
    {
      if (!monitorConnected) Serial.println("MASTER DISCONNECTED LONG TIME");
      else Serial.println("MDLT");
    }
  }
  else // Master Connected
  {
    if (Serial.available() > 0) // команда принята от оператора
    {
      String input = Serial.readStringUntil('\n');
      if (input.startsWith("CC")) // операторский старт-байт
      {
        for (int i = 0; i < gadgetCount; i++)
        {
          if (input[i + 2] == '5')
          {
            passGStates[i] = true;
            digitalWrite(13, HIGH);
            delay(10);
            digitalWrite(13, LOW);
          }
          else passGStates[i] = false;
        }
        if (input.endsWith("FF")) // Данные приняты от оператора
        {
          digitalWrite(13, HIGH);
          // Prepare to send states to Master
          digitalWrite(serialTXControl, HIGH);  // передаем состояния мастеру
          masterSerial.write(0xBB);
          delay(10);
          //Sending...
          for (int d = 0; d < gadgetCount; d++)
          {
            if (passGStates[d]) masterSerial.write(0x05);
            else masterSerial.write(0x01);
            delay(10);
          }
          masterSerial.write(0xFF); // конец передачи состояний мастеру
          delay(10);
          digitalWrite(serialTXControl, LOW);  // Stop Transmitter
          digitalWrite(13, LOW);
        }
      }
      else if (input.startsWith("CD")) // Прием от мастера голосовых подсказок
      {
        Serial.println(input);
        int endCD = input.indexOf("FF");
        if (endCD > 1)
        {
          int voiceIndextoSend = input.substring(2, endCD).toInt();
          byte sendVoiceByte = lowByte(voiceIndextoSend);
          digitalWrite(13, HIGH);
          // Prepare to send states to Master
          digitalWrite(serialTXControl, HIGH);  // передаем состояния мастеру
          masterSerial.write(0xBD);
          delay(10);
          //Sending...
          masterSerial.write(sendVoiceByte);
          delay(10);
          masterSerial.write(0xFF); // конец передачи состояний мастеру
          delay(10);
          digitalWrite(serialTXControl, LOW);  // Stop Transmitter
          digitalWrite(13, LOW);
        }
      }
      else if (input.startsWith("CF"))
      {
        Serial.println(input);
        int endCD = input.indexOf("FF");
        if (endCD > 1)
        {
          digitalWrite(13, HIGH);
          // Prepare to send states to Master
          digitalWrite(serialTXControl, HIGH);
          masterSerial.write(0xBF);
          delay(10);
          byte light = 0x00;
          if(input.substring(2,4) == "LU") light = 0x05;
          else if (input.substring(2,4) == "LD") light = 0x01;
          masterSerial.write(light);
          delay(10);
          masterSerial.write(0xFF);
          delay(10);
          digitalWrite(serialTXControl, LOW);  // Stop Transmitter
          digitalWrite(13, LOW);
        }
      }
      else if (input.startsWith("ClearStates")) // прием команды оператора на сброс состояний
      {
        digitalWrite(13, HIGH);
        // Prepare to send states to Master
        digitalWrite(serialTXControl, HIGH);  // Init
        masterSerial.write(0xBC);
        delay(10);
        digitalWrite(serialTXControl, LOW);  // Stop
        digitalWrite(13, LOW);
      }
      else Serial.flush();
    }

    if (masterSerial.available() > 0) //прием команды от мастера
    {
      byte input[gadgetCount];
      for (byte i = 0; i < gadgetCount; i++) input[i] = 0x00;
      byte inByte = masterSerial.read();

      //masterSerial.println("First byte is " + String(inByte));
      if (inByte == 0xA1)
      {
        masterConnected = false;
        for (int s = 0; s < gadgetCount; s++) passGStates[s] = false;
      }
      else if (inByte == 0xA9) // контроль подключения
      {
        mLastA9Rec = tick;
      }
      else if (inByte == 0xA5) // сброс состояний со стороны мастера
      {
        for (int s = 0; s < gadgetCount; s++) passGStates[s] = false;
        if (!monitorConnected) Serial.println("Send to operator clear states and run");
        Serial.write("masterStart\n");
        //Sending...
      }
      else if (inByte == 0xAA) // информация о гаджетах
      {
        delay(350);
        if (!monitorConnected) Serial.print("Recieving data from master: ");
        for (int i = 0; i < gadgetCount; i++)
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
          String toOperator = "BB";
          Serial.write("BB");
          for (int d = 0; d < gadgetCount; d++)
          {
            if (passGStates[d]) Serial.write("5");
            else Serial.write("1");
          }
          Serial.write("FF\n");
          if (!monitorConnected) Serial.println(); // for test only
        }
      }
      while (masterSerial.available()) masterSerial.read();
      if (tick - mLastA9Rec > masterConnTimeOut) // при отсутствии контрольного сигнала переподключаемся к мастеру
      {
        digitalWrite(serialTXControl, HIGH);  // Init Transmitter
        masterSerial.write(0xBC);
        delay(10);
        digitalWrite(serialTXControl, LOW);  // Stop Transmitter
      }
    }
  }
}
