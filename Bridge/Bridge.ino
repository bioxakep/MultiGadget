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
byte gStates[32]; //1 3 5
boolean monitorConnected = false;
boolean masterConnected = false;
unsigned long mConnectTime = 0;
unsigned long mDisconnectTime = 0;
unsigned long mLastA9Rec = 0;
unsigned long masterConnTimeOut = 20000;
int gadgetCount = 32;
void setup() {
  masterSerial.begin(9600);
  Serial.begin(9600);
  pinMode(serialTXControl, OUTPUT);
  digitalWrite(serialTXControl, LOW);
  for (int s = 0; s < gadgetCount; s++) gStates[s] = 0x01;
  connectToMonitor();
}

void loop()
{
  unsigned long tick = millis();
  // ==================== ПОДКЛЮЧЕНИЕ ====================
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
      if (input.startsWith("CD")) // информация о гаджетах
      {
        for (int i = 0; i < gadgetCount; i++)
        {
          if (input[i + 2] == '3') gStates[i] = 0x03;
          else gStates[i] = 0x01;
        }
        if (input.endsWith("FF")) // Данные приняты от оператора
        {
          // Prepare to send states to Master
          digitalWrite(serialTXControl, HIGH);  // передаем состояния мастеру
          masterSerial.write(0xBD);
          delay(10);
          //Sending...
          for (int d = 0; d < gadgetCount; d++)
          {
            digitalWrite(13, HIGH);
            masterSerial.write(gStates[d]);
            delay(10);
            digitalWrite(13, LOW);
          }
          masterSerial.write(0xFF); // конец передачи состояний мастеру
          delay(10);
          digitalWrite(serialTXControl, LOW);  // Stop Transmitter
        }
      }
      else if (input.startsWith("CA")) // Прием от оператора голосовых подсказок
      {
        Serial.println(input);
        int endCD = input.indexOf("FF");
        if (endCD > 1)
        {
          int voiceIndextoSend = input.substring(2, endCD).toInt();
          byte sendVoiceByte = lowByte(voiceIndextoSend);
          digitalWrite(13, HIGH);
          digitalWrite(serialTXControl, HIGH);  // передаем состояния мастеру
          masterSerial.write(0xBA);
          delay(10);
          masterSerial.write(sendVoiceByte);
          delay(10);
          masterSerial.write(0xFF); // конец передачи состояний мастеру
          delay(10);
          digitalWrite(serialTXControl, LOW);  // Stop Transmitter
          digitalWrite(13, LOW);
        }
      }
      else if (input.startsWith("CF")) // Управление светом
      {
        Serial.println(input);
        int endCD = input.indexOf("FF");
        if (endCD > 1)
        {
          digitalWrite(13, HIGH);// Начало передачи информации о состоянии подсветки
          digitalWrite(serialTXControl, HIGH);
          masterSerial.write(0xBF);
          delay(10);
          byte light = 0x00;
          if(input.indexOf("LU") > 0) light = 0x05;
          else if (input.indexOf("LD") > 0) light = 0x01;
          masterSerial.write(light);
          delay(10);
          masterSerial.write(0xFF);
          delay(10);
          digitalWrite(serialTXControl, LOW);  // Stop Transmitter
          digitalWrite(13, LOW);// Конец передачи информации о состоянии подсветки
        }
      }
      else if (input.startsWith("CC")) // прием команды оператора на сброс состояний
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
    // =========== КОМАНДЫ ОТ МАСТЕРА ===========
    if (masterSerial.available() > 0) //прием команд от мастера
    {
      byte input[gadgetCount];
      for (byte i = 0; i < gadgetCount; i++) input[i] = 0x00;
      byte inByte = masterSerial.read();

      if (inByte == 0xA1) // сброс состояний
      {
        masterConnected = false;
        for (int s = 0; s < gadgetCount; s++) gStates[s] = 0x01;
      }
      else if (inByte == 0xA9) // контроль подключения (периодический сигнал синхронизации)
      {
        mLastA9Rec = tick;
      }
      else if (inByte == 0xAC) // сброс состояний со стороны мастера
      {
        for (int s = 0; s < gadgetCount; s++) gStates[s] = 0x01;
        if (!monitorConnected) Serial.println("Send to operator clear states and run");
        Serial.write("masterStart\n");
      }
      else if (inByte == 0xAD) // Принимаем информацию о гаджетах с мастера
      {
        delay(350);
        if (!monitorConnected) Serial.print("Recieving data from master: ");
        for (int i = 0; i < gadgetCount; i++)
        {
          input[i] = masterSerial.read();
          //Serial.print("|" + String(input[i])); // DEBUG
          gStates[i] = input[i];
        }
        if (!monitorConnected) Serial.println();
        byte last = masterSerial.read();
        //Serial.println("Last byte is " + String(last));
        if (last == 0xFF)
        {
          if (!monitorConnected) Serial.print("Send to operator: "); // Отправляем информацию о гаджетах оператору
          Serial.write("BD");
          for (int d = 0; d < gadgetCount; d++)
          {
            if (gStates[d] == 0x05) Serial.write("5");
            else if (gStates[d] == 0x03) Serial.write("3");
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
