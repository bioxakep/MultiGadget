int gateBeaconA   = 513;
int gateBeaconB   = 626;
int underBeaconA  = 396;
int underBeaconB  = 385;


boolean getWindRFID() {
  if(digitalRead(windRFPin)==LOW)
  { 
    return true; 
    }else return false;
}
 

boolean getUnderRFID() {
  cpz2->check();
  if(cpz2->isBeaconConnecting())
  {
    byte addr[6];
    int ficha2 = 0;
    cpz2->getAddress(addr);
    //Serial.print("\nBeacon-Under: ");
    for(int i = 0; i < 6; i++) 
    {                               
      ficha2 = ficha2 + addr[i];    // Serial.print(addr[i], HEX);
    }
    //Serial.print(" sum = ");
    //Serial.println(ficha2);
    if (ficha2 == underBeaconA || ficha2 == underBeaconB) return true;
    else return false;
  }
  else return false;
}
boolean getGateRFID() {
  cpz3->check();
  if(cpz3->isBeaconConnecting())
  {
    byte addr[6];
    int ficha3 = 0;
    cpz3->getAddress(addr);
    //Serial.print("Beacon-gate: ");
    for(int i = 0; i < 6; i++) 
    {                               
      ficha3 = ficha3 + addr[i];    // Serial.print(addr[i], HEX);
    }
    //Serial.print(" sum = ");
    //Serial.println(ficha3);
    if (ficha3 == gateBeaconA || ficha3 == gateBeaconB) return true;
    else return false;
  }
  else return false;
}

boolean getRainRFID() {
  if(digitalRead(rainRFPin)==LOW)
  { 
    return true; 
    }else return false;
}
