void Start()
{
  if (!startStates[0] && startStates[1])
  {
    start++;   // wait for prestart signal
    if (start == 1)
    {
      Serial.println("\nFirst START pressed, all locks closed...");
      sendToSlave(motorConAddr, 0x10);
      // поднимает облако [cloud up], 
      // поднимает колонну[column up],
      // поднимают виноград [grape up],
      // шторки спускает
      sendToSlave(lightConAddr, 0x10);
      closeLocks();
      lcd.clear();
      lcd.print("Prestart done.");
      send250ms(gorgoOUT);  // activate gorgona
      mp3_set_serial(Serial);
      mp3_play(100); // prestart message
    }
    else if (start == 2)       //start game
    {
      mp3_set_serial(Serial);
      mp3_play(111); // First Soundtrack (last for master)
      delay(200);
      sendToSlave(lightConAddr, 0x11); // send random wind to lightController
      sendToSlave(motorConAddr, 0x11); // lift the cloud
      Serial.println("Go to level 12");
      level = 12;
      digitalWrite(SSerialTxControl, HIGH);  // Init Transmitter
      Serial1.write(0xA5);
      delay(10);
      lcd.clear();
      lcd.print("Timer started     Ballon level");
      digitalWrite(SSerialTxControl, LOW);  // Stop Transmitter
      Serial.println("\n*************************************** Game STARTed ******************************************\n");
    }
  }
}

void Baloon()
{
  if ((!digitalRead(balloIN) || operGStates[baloon]) && !passGStates[baloon]) //signal from finished ballon received
  {
    passGStates[baloon] = true;
    if (operGStates[baloon]) send250ms(balloOUT);
    Serial.println("Ballon signal recieved. Turn on the lights and roll up the curtain");
    // send (i2c) signal to motor_controller >rollUp and light_controller  >lights and stop wind
    sendToSlave(lightConAddr, 0x12); // wind off + lights on
    send250ms(pressOUT);
    send250ms(gheraOUT);  // moves ghera to level 10
    level = 13;
    Serial.println("Go to level 13");
    sendToSlave(motorConAddr, 0x12);  // turn on the lights
    delay(1000);
    mp3_stop();
  }
}

void Press()
{
  if ((!digitalRead(pressIN) || operGStates[presss]) && !passGStates[presss]) //victory signal from press received
  {
    passGStates[presss] = true;
    if (operGStates[presss]) send250ms(pressOUT);
    Serial.println("Press signal recieved. Press gave players the RFID key to the gate ");
    level = 14;
    Serial.println("Go to level 14");
  }
}

void Gate()
{
  gateRFWait = !getGateRFID();
  //Serial.println("gateWait = " + String(gateRFWait));
  //Serial.println("gatePassState = " + String(passGStates[gate]));
  if ((!gateRFWait || operGStates[gate]) && !passGStates[gate])
  {
    passGStates[gate] = true;
    sendToSlave(motorConAddr, 0x14); // send signal to motor_controller >openGate
    level = 90;
    Serial.println("Gate Done, command 0x14 sent to motor and light , Go to level 90");
    sendToSlave(lightConAddr, 0x14); // send signal to light_controller >turnOnTheLights
    mp3_play(1);
    send250ms(gheraOUT);  // ghera start speaking, shift Ghera forward - 'thunder' level
  }
}

void Poseidon()
{
  if ((!digitalRead(poseiIN) || operGStates[poseidon]) && !passGStates[poseidon])
  { // signal from poseidon
    // posei > command via (i2c) to motor_controller activate falling column > players get first part thundi
    // shoud be skippable from master console
    sendToSlave(motorConAddr, 0x20); // Открываем тайник Посейдона, даем игрокам трезубец
    passGStates[poseidon] = true;
    if (operGStates[poseidon]) send250ms(poseiOUT);
    Serial.println("Poseidon Done");
    mp3_play(5);
    delay(100);
    sendToSlave(motorConAddr, 0x20); // Открываем тайник Посейдона, даем игрокам трезубец -- REPEAT
  }
}

void Trident()
{
  if ((!digitalRead(triPin) || operGStates[trident]) && !passGStates[trident] && passGStates[poseidon])
  {
    mp3_play(4); // START MP3 - FILE
    passGStates[trident] = true;
    tridentTimer = millis();
    Serial.println("Trident started " + String(millis()));
  }
}

void Demetra()
{
  if ((!digitalRead(demetIN) || operGStates[demetra]) && !passGStates[demetra])
  {
    passGStates[demetra] = true;
    if (operGStates[demetra]) send250ms(demetOUT);
    Serial.println("Demetra 1 part Done");
    demetTimer = millis();
  }
}

void Rain()
{
  if ((!rainRFWait || operGStates[rain]) && !passGStates[rain] && passGStates[demetra])
  {
    sendToSlave(motorConAddr, 0x22); // send signal to motor_controller > grapeGrow
    passGStates[rain] = true;
    mp3_play(3);     // MP3 FILE rain
    Serial.println("Rain Done");
    delay(100);
    sendToSlave(motorConAddr, 0x22); // send signal to motor_controller > grapeGrow   REPEAT !!!! just in case
  }
}

void Vine()
{
  if ((!digitalRead(vinemIN) || operGStates[vine]) && !passGStates[vine]) // && passGStates[demetra] && passGStates[rain])
  {
    passGStates[vine] = true;
    if (operGStates[vine]) send250ms(vinemOUT);
    send250ms(dioniOUT);  // inform Dionis the vine in done
    Serial.println("Vine Done");
  }
}

void Dionis1()
{
  if ((!digitalRead(dioniIN) || operGStates[dionis1]) && !passGStates[dionis1]) //&& passGStates[vine])
  { // if players gives dionis empty bottle
    // he will tell them that he dont like empty bottles
    // if signal from full bottle is received >
    // dioniHD1 opens > players gets another thunder
    if (operGStates[dionis1]) send250ms(dioniOUT);
    passGStates[dionis1] = true;
    // MP3 FILE
    digitalWrite(dioniHD1, HIGH); // open first dionis vault
    Serial.println("Dionis-1 Done");
    delay(300);  //rem on dec 3 to test - bad
                 // delay while finish first command from dionis
  }
}


void Narcis()
{
  if ((!digitalRead(narciIN) || operGStates[narcis]) && !passGStates[narcis])
  {  
    if (operGStates[narcis]) send250ms(narciOUT);
    passGStates[narcis] = true;
    Serial.println("Narcis Done");
  }
}

void Thunder()
{
  if ((!digitalRead(thundIN) || operGStates[thunder]) && !passGStates[thunder])
  { // shoud be triggerable from master console
    // may add some extra storm effects thru light_controller
    //if all thunders are done >   // gheraLevel = 2 >  speaks > opend HD2 (shields)
    send250ms(gheraOUT);  // moves ghera to 'shields' level, ALWAYS OR BY OPERATOR?
    sendToSlave(lightConAddr, 0x23); // send signal to light_controller > GheraSpeaks, dim the light
    passGStates[thunder] = true;
    thunderDone = true;
    mp3_play(88);     // play mp3 thunder
    Serial.println("Thunder Done");
  }
}
void Afina1()
{
  if ( (!digitalRead(afinaIN) || operGStates[afina1] ) && !passGStates[afina1])
  { // afina  first signal > open afinaHD1  > players get knife
    
    if (operGStates[afina1]) send250ms(afinaOUT);
    passGStates[afina1] = true;
    afinaTimer = millis();
    Serial.println("Afina 1 part Done > small vault open");
    delay(350);
    // here afina gives players a key to open next vault
  }
}

void Afina2()
{
  if ( (!digitalRead(afinaIN) || operGStates[afina2] ) && !passGStates[afina2]) //(!digitalRead(afinaIN) ||
  { // afina second signal opens afinaHD2
    //if (operGStates[afina2]) send250ms(afinaOUT);
    digitalWrite(afinaHD2, LOW);
    passGStates[afina2] = true;
    Serial.println("Afina 2 part Done > Large vault open");
    delay(200);
  }
}

void TimeG()
{
  if ((!digitalRead(timeIN) || operGStates[Time]) && !passGStates[Time])
  {
    if (operGStates[Time]) send250ms(timeOUT);
    passGStates[Time] = true;
    Serial.println("Time part Done");
  }
}

void Octopus()
{
  if ((!digitalRead(octopIN) || operGStates[octopus]) && !passGStates[octopus]) // && passGStates[Time])
  { 
    if (operGStates[octopus]) send250ms(octopOUT);
    passGStates[octopus] = true;
    Serial.println("Octopus Done");
  }
}

void Note()
{
  if ((!digitalRead(noteIN) || operGStates[note]) && !passGStates[note])
  {
    // note > players get wind amulet
    if (operGStates[note]) send250ms(noteOUT);
    digitalWrite(noteHD, LOW);    
    Serial.println("Note Done");
    passGStates[note] = true;
  }
}

void Wind()
{
  if ((!windRFWait || operGStates[wind]) && !passGStates[wind]) //&& passGStates[note]) //disc
  { //Из тайника игроки получают рфид ветра.и когда вставляют его в приемник рфида ветра то включаем ветер на 10-15 секунд,
    //мр3 файл и спускаем облакоИз тайника игроки получают рфид ветра.и когда вставляют его в приемник рфида
    //ветра то включаем ветер на 10-15 секунд, мр3 файл и спускаем облако
    passGStates[wind] = true;
    Serial.println("Wind RFID Recieved");
    sendToSlave(motorConAddr, 0x31); // CloudDown - отдать 3 часть щита
    delay(50);
    sendToSlave(lightConAddr, 0x31); // windBlow 10-15 secs
    mp3_play(2);    // Wind MP3 FILE
  }
}

void Ghera1() // SHIELDS done
{
  if ((!digitalRead(gheraIN) || operGStates[ghera1]) && !passGStates[ghera1])
  { //if all shields in ghera are in  place (gheraLevel = 40 ) gera speaks - open sealHD
    mp3_play(12);
    if (operGStates[ghera1]) send250ms(gheraOUT);
    passGStates[ghera1] = true;
    delay(50);
    send250ms(musesOUT);
    delay(50);
    // all gods to traitor level (level 40)
    send250ms(dioniOUT); 
    delay(50);
    send250ms(demetOUT); 
    delay(50);
    send250ms(afinaOUT); 
    delay(50);
    send250ms(noteOUT); 
    delay(50);
    send250ms(poseiOUT); 
    delay(50);
    passGStates[afina2] = true; 
    Serial.println("Ghera level 30 Done, SHIELDs Done");
    sendToSlave(lightConAddr, 0x30); // turn lights off while Ghera speaks
  }
}

void Fire()
{
  if ((!digitalRead(firePin) || operGStates[fire]) && !passGStates[fire])     
  {
    mp3_play(77);
    sendToSlave(lightConAddr, 0x41); //turner start  - dim the room light 
    passGStates[fire] = true;
    Serial.println("Fire Done, turner start...");
  }
}

void Flower1()  // first level of flower
{
  if ((!digitalRead(flowrIN) || operGStates[flower1]) && !passGStates[flower1])
  { 
    mp3_play(6);
    if (operGStates[flower1]) send250ms(flowrOUT);
    passGStates[flower1] = true;
    Serial.println("Flower-1 Blue part Done");
    delay(300);
  }
}

void Flower2()    // second level of flower
{
  if ((!digitalRead(flowrIN) || operGStates[flower2]) && !passGStates[flower2] && passGStates[flower1])
  { 
    mp3_play(7);
    delay(50);
    sendToSlave(lightConAddr, 0x42); // turner off, light switch 
    if (operGStates[flower2]) send250ms(flowrOUT);
    passGStates[flower2] = true;
    Serial.println("Flower-2 RED part Done");
    flowerTimer = millis();
  }
}




void Dionis2()  // dionis(2) cold heart if all done
{ 
  if ((!digitalRead(dioniIN) || operGStates[dionis2]) && !passGStates[dionis2] && passGStates[ghera1])
  { 
    if (operGStates[dionis2]) send250ms(dioniOUT);
    passGStates[dionis2] = true;
    delay(50);
    send250ms(demetOUT); 
    delay(50);
    send250ms(afinaOUT); 
    delay(50);
    send250ms(noteOUT); 
    delay(50);
    send250ms(poseiOUT); 
    delay(50);
   // send250ms(dioniOUT); // send final signal  to shut up everybody
    Serial.println("Dionis-2 Done");
    digitalWrite(dioniHD2, LOW);
  }
}

void Ghera2()   // SEALS done
{
  if (passGStates[flower2] && passGStates[arpha] && passGStates[dionis2])
  {
    if ((!digitalRead(gheraIN) || operGStates[ghera2]) && !passGStates[ghera2])
    {
      if (operGStates[ghera2]) send250ms(gheraOUT);
      passGStates[ghera2] = true;
      sealsDone = true;
      Serial.println("Seals Done");
      delay(50);
      // if all seals are in ghera are in place  > ghera Level = 60   >>   sealsDone = true;
      // ((((  ghera speaks > open HD3 (key for underground) ))))
      sendToSlave(lightConAddr, 0x45); // turn lights off while Ghera speaks
      mp3_play(13);
      delay(3000);
    }
  }
}

void BigKey()
{
  if ((digitalRead(bigKeyIN) || operGStates[bigkey]) && !passGStates[bigkey])
  { 
    sendToSlave(motorConAddr, 0x51);
    mp3_play(8);
    passGStates[bigkey] = true;
    Serial.println("BigKey Done");
  }
}

void Underground()
{
  underRFWait = !getUnderRFID();
  if ((!underRFWait || operGStates[under]) && !passGStates[under])
  {
    sendToSlave(motorConAddr, 0x53);
    mp3_play(9);
    passGStates[under] = true;
    send250ms(minotOUT);  // activate Minotavr
    send250ms(narciOUT);  // shift Narcis to show underground map
    underRFWait = false;
    Serial.println("Underground Opened");
  }
}


void Minotavr()
{
  if ((!digitalRead(minotIN) || operGStates[minot]) && !passGStates[minot] && passGStates[under])
  { 
    passGStates[minot] = true;
    Serial.println("Minot Done");
    minotTimer = millis();
  }
}

void Gorgona()
{
  if ((!digitalRead(gorgoIN) || operGStates[gorgona]) && !passGStates[gorgona] && passGStates[under])
  { 
    passGStates[gorgona] = true;
    Serial.println("Gorgona Done");
    digitalWrite(gorgoHD, HIGH);
    delay(220);
    digitalWrite(gorgoHD, LOW);
  }
}

void Crystals()
{
  if (passGStates[gorgona] && passGStates[minot])
  {
      Wire.requestFrom(lightConAddr, 1);
      byte lightUnsw = Wire.requestFrom(lightConAddr, 1, false);
      boolean crystCheck = true;
      int cristSum = 0;
      if (lightUnsw == 1)
      {
        byte unswer = Wire.read();
        Serial.println("CRYST STATES= " + String(unswer, BIN));
        for (int u = 0; u < 3; u++)
        {
          crystStates[u] = 1 & (unswer >> 2 * u);
          if (crystStates[u]) cristSum++;
          crystCheck &= crystStates[u];
        }
        if (crystCheck) passGStates[crystals] = true;
      }
      if(cristSum != cristCount)
      {
        if(cristSum > 0) mp3_play(10);
        cristCount = cristSum;
      }
      if (passGStates[crystals] || operGStates[crystals])
       {
        // mp3_play(10); //each one (1-2)
         delay(100);
         mp3_play(11); // then all 3
  
         //level = 100;
         Serial.println("Crystals Done");
         sendToSlave(motorConAddr, 0x99);  // game over = stop the underground music
         send250ms(gheraOUT); // victory signal for Ghera
          // turn lights off while Ghera speaks
          if (operGStates[crystals]) 
           {
            passGStates[crystals] = true; 
            sendToSlave(lightConAddr, 0x55); // inform light_controller than we skipping crystals
           }
        }
  }
}

void Octopus2()
{
   if ((!digitalRead(octopIN) || operGStates[octopus2]) && !passGStates[octopus2]))
  { 
    if (operGStates[octopus2]) send250ms(octopOUT);
    passGStates[octopus2] = true;
    Serial.println("Octopus-2 Done");
  }
}

void Hercules()
{
  if ((!digitalRead(hercuIN) || operGStates[hercul]) && !passGStates[hercul])
  { 
    // hercu > players gets third part of thunders
    passGStates[hercul] = true;
    mp3_play(66);
    openHercuHD();
    Serial.println("Hercules Done");
  }
}

void Arpha()   //  signal from arpha received
{
  if ((!digitalRead(musesIN) || operGStates[arpha]) && !passGStates[arpha])
  { 
    send250ms(musesOUT); // send signal to PLAY 998 AND shut up the muses
    if (operGStates[arpha]) send250ms(musesOUT);
    arphaTimer = millis();
    passGStates[arpha] = true;
    Serial.println("Arpha Done + signal to muses sent");

  }
}

void Zodiak()
{
  if ((!digitalRead(zodiaIN) || operGStates[zodiak]) && !passGStates[zodiak] && passGStates[under])
  { 
    sendToSlave(motorConAddr, 0x55);
    passGStates[zodiak] = true;
    Serial.println("Zodiak Done");
    digitalWrite(zodiaHD, HIGH);
    delay(220);
    digitalWrite(zodiaHD, LOW);
  }
}

