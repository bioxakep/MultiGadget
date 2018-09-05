#include <OneWire.h>
#include <ArdCPZ.h>
#include "Wire.h" // I2C

// WORLD v3 (beta)
// separate board for IRtx and HC-12 RX
// i2c conection to Master and other controllers
/*
  World  связан с Master по (i2c)
  есть четыре главные части : Факел(torch), Ветер(wind), Дождь(rain) и Тризубец(trident)

  Тризубец это всего лишь концевик который замыкаеться при установки тризубца
  в соответсвующее углубление > посылаем команду (i2c) мастеру

  Дождь, при считывания метки через rainRF отправляем мастеру команду 'rain'

  Ветер, при считывания метки через windRF включаем 10 светодтодов на ленте fastled и ждем сигнала
  с датчика windSens, если получаем сигнал то отправляем команду  'wind' на мастер

  Факел, посоянно посылаем сигнал ИК через пин ir, ждем команды от приемника НС-12 по Serial2 если получаем то
  отправляем мастеру команду 'torch' и начинаем наблюдать за пином turner.
  turner считает обороты крутилки, один сигнал за оборот, каждые три сигнала меняем  позицию
  1.North
  2.West
  3.South
  4.East
  1.North, etc.
  посылаем команду мастеру чтобы тот переслал ее light_controller
  плюс если позтция = 2.West то включаем выход redflower, если позтция = 4.East то включаем выход bluflower
  во всех остальных позициях оба (redflower и blueflower) выключенны
*/
int worldAdd = A0;
int turner   = A3;
int fastled  = 7;
int redflower = 53;
int bluflower = 51;
int location = 1;
int thisI2CAddr = 22;
void setup()
{
  Serial.begin(9600);
  Wire.begin(thisI2CAddr);
  Wire.onRequest(requestEvent);
  Serial.println("\n World_v3 (World_ADD required  AGO 2018");
  pinMode(redflower, OUTPUT);
  pinMode(bluflower, OUTPUT);
  pinMode(worldAdd, INPUT_PULLUP);
  pinMode(turner,   INPUT_PULLUP);
  pinMode(windSens, INPUT_PULLUP);
  pinMode(trident , INPUT_PULLUP);

  Serial.println ("worldAdd = " + String(digitalRead(worldAdd)));
  Serial.println ("turner   = " + String(digitalRead(turner)));
  Serial.println ("windSens = " + String(digitalRead(windSens)));
  Serial.println ("trident  = " + String(digitalRead(trident)));

  Serial.println("\n -----------------------------------------------");
}

void loop() {
  // if signal from worldAdd (torch) is recieved then start reacting to turner
  // every 3 signals from turner move location to next ( 1,2,3 or 4)
  // and send signal to master to shift light controller to corresonding level

  // if signal from trident (gerkon) is received > send signal to master (trident)

  // if signal from RFID rain is received > send signal to master (rain)

  // if signal from RFID wind is received > light up some fastled leds (10 aprox) and start reading windSensor

  // if signal from winSensor is received > send signal to master (wind)

}

void requestEvent()
{
}

