void checkRFID() {
  
  cpz1->check();

  if(cpz1->isBeaconConnecting())
  {
    byte addr[6];
    int ficha1 = 0;
    cpz1->getAddress(addr);
    Serial.print("\nBeacon-1: ");
    for(int i = 0; i < 6; i++) 
    {                               
      ficha1 = ficha1 + addr[i];    // Serial.print(addr[i], HEX);
    }
    Serial.print(" suma= ");
    Serial.println(ficha1);
    if (ficha1 == startBeacon) startCard = true;
  }

  cpz2->check();

  if(cpz2->isBeaconConnecting())
  {
    byte addr[6];
    int ficha2 = 0;
    cpz2->getAddress(addr);
    Serial.print("\nBeacon-2: ");
    for(int i = 0; i < 6; i++) 
    {                               
      ficha2 = ficha2 + addr[i];    // Serial.print(addr[i], HEX);
    }
    Serial.print(" suma= ");
    Serial.println(ficha2);
    if (ficha2 == underBeacon) underOpen = true;
  }

  cpz3->check();

  if(cpz3->isBeaconConnecting())
  {
    byte addr[6];
    int ficha3 = 0;
    cpz3->getAddress(addr);
    Serial.print("\nBeacon-2: ");
    for(int i = 0; i < 6; i++) 
    {                               
      ficha3 = ficha3 + addr[i];    // Serial.print(addr[i], HEX);
    }
    Serial.print(" suma= ");
    Serial.println(ficha3);
    if (ficha3 == gateBeacon) gateOpen = true;
  }

}
