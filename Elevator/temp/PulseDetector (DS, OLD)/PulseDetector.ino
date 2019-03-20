//DS18B20
//433 MHz Sender
//Arduino Nano
#include <DallasTemperature.h>
#include <OneWire.h>
#include <SoftwareSerial.h>

#define ONE_WIRE_BUS 2
#define XBeeRX 7
#define XBeeTX 8
#define MINTEMP 30.0
#define ALARMCODE 0xBB

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
SoftwareSerial XBeeSend(XBeeRX, XBeeTX); // RX, TX
float temp = 0;

void setup() 
{
  Serial.begin(9600);
  XBeeSend.begin(9600);
  sensors.begin();
}

void loop() 
{
  sensors.requestTemperatures();
  delay(1000);
  XBeeSend.write(RCODE);
  temp = (float)sensors.getTempCByIndex(0);
  if(temp > MINTEMP)
  {
    int pulse = (int)map(temp,35.5,37.2,60,99);
    String temptosend = String(pulse);
    temptosend = "A"+temptosend;
    XBeeSend.println(temptosend);
  }
  else XBeeSend.write(ALARMCODE);
}
