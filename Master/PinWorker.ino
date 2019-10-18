void openLocks() {
  digitalWrite(balloOUT, LOW);
  digitalWrite(pressOUT, LOW);
  digitalWrite(demetOUT, LOW);
  digitalWrite(demetHD,  LOW);
  digitalWrite(vinemOUT, LOW);
  digitalWrite(dioniOUT, LOW);
  digitalWrite(dioniHD1, HIGH);  // HIGH to OPEN
  digitalWrite(dioniHD2, LOW);
  digitalWrite(hercuHD,  LOW);
  digitalWrite(thundOUT, LOW);
  digitalWrite(poseiOUT, LOW);
  digitalWrite(narciOUT, LOW);
  digitalWrite(noteOUT,  LOW);
  digitalWrite(noteHD,   LOW);
  digitalWrite(octopOUT, LOW);
  digitalWrite(afinaOUT, LOW);
  digitalWrite(afinaHD1, LOW);
  digitalWrite(afinaHD2, LOW);
  digitalWrite(musesOUT, LOW);
  digitalWrite(arphaHD,  LOW);
  digitalWrite(gorgoOUT, LOW);
  digitalWrite(gorgoHD,  HIGH);
  digitalWrite(minotOUT, LOW);
  digitalWrite(minotHD,  HIGH);
  digitalWrite(zodiaHD,  HIGH);
  digitalWrite(flowrOUT, LOW);
  digitalWrite(gheraOUT, LOW);
  delay(200);
  digitalWrite(hercuHD,  LOW);
  digitalWrite(gorgoHD,  LOW);
  digitalWrite(minotHD,  LOW);
  digitalWrite(zodiaHD,  LOW);
  digitalWrite(flowrHD,  LOW);
  digitalWrite(spare,    LOW);
  
} // eof_openlocks

void closeLocks() {
  digitalWrite(balloOUT, LOW);
  digitalWrite(pressOUT, LOW);
  digitalWrite(demetOUT, LOW);
  digitalWrite(demetHD,  HIGH);
  digitalWrite(vinemOUT, LOW);
  digitalWrite(dioniOUT, LOW);
  digitalWrite(dioniHD1, LOW);
  digitalWrite(dioniHD2, HIGH);
  digitalWrite(hercuHD,  HIGH);
  digitalWrite(thundOUT, LOW);
  digitalWrite(poseiOUT, LOW);
  digitalWrite(narciOUT, LOW);
  digitalWrite(noteOUT,  LOW);
  digitalWrite(noteHD,   HIGH);
  digitalWrite(octopOUT, LOW);
  digitalWrite(afinaOUT, LOW);
  digitalWrite(afinaHD1, HIGH);
  digitalWrite(afinaHD2, HIGH);
  digitalWrite(musesOUT, LOW);
  digitalWrite(arphaHD,  HIGH);
  digitalWrite(gorgoOUT, LOW);
  digitalWrite(gorgoHD,  HIGH);
  digitalWrite(minotOUT, LOW);
  digitalWrite(minotHD,  HIGH);
  digitalWrite(zodiaHD,  HIGH);
  digitalWrite(flowrOUT, LOW);
  digitalWrite(gheraOUT, LOW);
  delay(200);
  digitalWrite(gorgoHD,  LOW);
  digitalWrite(minotHD,  LOW);
  digitalWrite(zodiaHD,  LOW);
  digitalWrite(poseiHD,  LOW);
  digitalWrite(flowrHD,  HIGH);
  digitalWrite(spare,    HIGH);
  Serial.println("locks Closed");
} // eof_closeLocks

void openOpened()
{
  if (gStates[zodiak])  digitalWrite(zodiaHD, HIGH);
  if (gStates[minot])   digitalWrite(minotHD, HIGH);
  if (gStates[gorgona]) digitalWrite(gorgoHD, HIGH);
  delay(170);
  digitalWrite(zodiaHD, LOW);
  digitalWrite(minotHD, LOW);
  digitalWrite(gorgoHD, LOW);
}

void checkInputs()
{
  Serial.print(" timeIN  = "); Serial.print  (digitalRead(timeIN) ? "HIGH" : "LOW ");
  Serial.print(" balloIN = "); Serial.println (digitalRead(balloIN) ? "HIGH" : "LOW ");
  Serial.print(" pressIN = "); Serial.print   (digitalRead(pressIN) ? "HIGH" : "LOW ");
  Serial.print(" demetIN = "); Serial.println (digitalRead(demetIN) ? "HIGH" : "LOW ");
  Serial.print(" vinemIN = "); Serial.print  (digitalRead(vinemIN) ? "HIGH" : "LOW ");
  Serial.print(" dioniIN = "); Serial.println(digitalRead(dioniIN) ? "HIGH" : "LOW ");
  Serial.print(" hercuIN = "); Serial.print  (digitalRead(hercuIN) ? "HIGH" : "LOW ");
  Serial.print(" thundIN = "); Serial.println(digitalRead(thundIN) ? "HIGH" : "LOW ");
  Serial.print(" poseiIN = "); Serial.print  (digitalRead(poseiIN) ? "HIGH" : "LOW ");
  Serial.print(" narciIN = "); Serial.println(digitalRead(narciIN) ? "HIGH" : "LOW ");
  Serial.print("  noteIN = "); Serial.print  (digitalRead( noteIN) ? "HIGH" : "LOW ");
  Serial.print(" octopIN = "); Serial.println(digitalRead(octopIN) ? "HIGH" : "LOW ");
  Serial.print(" afinaIN = "); Serial.print  (digitalRead(afinaIN) ? "HIGH" : "LOW ");
  Serial.print(" musesIN = "); Serial.println(digitalRead(musesIN) ? "HIGH" : "LOW ");
  Serial.print(" gorgoIN = "); Serial.print(digitalRead(gorgoIN) ? "HIGH" : "LOW ");
  Serial.print(" minotIN = "); Serial.println  (digitalRead(minotIN) ? "HIGH" : "LOW ");
  Serial.print(" zodiaIN = "); Serial.print(digitalRead(zodiaIN) ? "HIGH" : "LOW ");
  Serial.print(" flowrIN = "); Serial.println  (digitalRead(flowrIN) ? "HIGH" : "LOW ");
  Serial.print(" gheraIN = "); Serial.print(digitalRead(gheraIN) ? "HIGH" : "LOW ");
  Serial.print(" triPin  = "); Serial.println  (digitalRead(triPin)  ? "HIGH" : "LOW ");
  Serial.print(" windRF  = ");  Serial.print (digitalRead(windRFPin) ? "HIGH" : "LOW ");
  Serial.print(" rainRF  = "); Serial.println  (digitalRead(rainRFPin)  ? "HIGH" : "LOW ");
  Serial.print(" voicePi = "); Serial.print  (digitalRead(voicePin) ? "HIGH" : "LOW ");
  Serial.print(" bigKey  = "); Serial.println  (digitalRead(bigKeyIN) ? "HIGH" : "LOW ");
}

boolean debounce(boolean prevstate , int pin)
{
  boolean currstate = digitalRead(pin);
  if (currstate != prevstate)
  {
    delay(10);
    currstate = digitalRead(pin);
  }
  return currstate;
}

void pinSetup()
{
  pinMode(voicePin, INPUT_PULLUP);
  pinMode(windRFPin, INPUT_PULLUP);
  pinMode(rainRFPin, INPUT_PULLUP);
  
  pinMode(startPin, INPUT_PULLUP);
  pinMode( balloIN  , INPUT_PULLUP);
  pinMode( balloOUT , OUTPUT);

  pinMode( pressIN  , INPUT_PULLUP);
  pinMode( pressOUT , OUTPUT);

  pinMode( demetIN  , INPUT_PULLUP);
  pinMode( demetOUT , OUTPUT);
  pinMode( demetHD  , OUTPUT);

  pinMode( vinemIN  , INPUT_PULLUP);
  pinMode( vinemOUT , OUTPUT);    // click-lock on prop,

  pinMode( dioniIN  , INPUT_PULLUP);
  pinMode( dioniOUT , OUTPUT);
  pinMode( dioniHD1 , OUTPUT); // piston show up
  pinMode( dioniHD2 , OUTPUT);

  pinMode( hercuIN  , INPUT_PULLUP);
  pinMode( hercuHD  , OUTPUT);

  pinMode( thundIN  , INPUT_PULLUP);
  pinMode( thundOUT , OUTPUT);

  pinMode( poseiIN  , INPUT_PULLUP);
  pinMode( poseiOUT , OUTPUT);
  pinMode( poseiHD  , OUTPUT);

  pinMode( narciIN  , INPUT_PULLUP);
  pinMode( narciOUT , OUTPUT);     /// launch video

  pinMode( noteIN   , INPUT_PULLUP);
  pinMode( noteOUT  , OUTPUT);
  pinMode( noteHD   , OUTPUT);

  pinMode( octopIN  , INPUT_PULLUP);
  pinMode( octopOUT , OUTPUT);    // click-lock on prop

  pinMode( afinaIN  , INPUT_PULLUP);
  pinMode( afinaOUT , OUTPUT);

  pinMode( afinaHD1 , OUTPUT);
  pinMode( afinaHD2 , OUTPUT);

  pinMode( musesIN  , INPUT_PULLUP);
  pinMode( musesOUT , OUTPUT);

 // pinMode( arphaIN  , INPUT_PULLUP);
  pinMode( arphaHD  , OUTPUT);

  pinMode( gorgoIN  , INPUT_PULLUP);
  pinMode( gorgoOUT , OUTPUT);
  pinMode( gorgoHD  ,  OUTPUT);  //click

  pinMode( minotIN  , INPUT_PULLUP);
  pinMode( minotOUT , OUTPUT);
  pinMode( minotHD  , OUTPUT); // click

  pinMode( zodiaIN  , INPUT_PULLUP);
  pinMode( zodiaHD  , OUTPUT); //click

  pinMode( flowrIN  , INPUT_PULLUP);
  pinMode( flowrOUT , OUTPUT);

  pinMode( gheraIN  , INPUT_PULLUP);
  pinMode( gheraOUT , OUTPUT);
  pinMode( flowrHD,   OUTPUT);

  pinMode(timeIN, INPUT_PULLUP);
  pinMode(timeOUT, OUTPUT);
  pinMode(octopIN, INPUT_PULLUP);

  pinMode(triPin, INPUT_PULLUP);
  pinMode(firePin, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  pinMode(spare, OUTPUT);
  pinMode(SSerialTxControl, OUTPUT);
  digitalWrite(SSerialTxControl, LOW);  // Init Recieve RS485
}
