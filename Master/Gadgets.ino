void Start()
{
  if (!startStates[0] && startStates[1])
  {
    start++;   // wait for prestart signal
    if (start == 1)
    {
      mp3_set_serial(Serial);
      mp3_play(2); // prestart message
      delay(200);
      Serial.println("\nFirst START pressed, all locks closed...");
      delay(10);
      sendToSlave(motorConAddr, 0x10);
      lcd.clear();
      lcd.print("Prestart done.");
      // поднимает облако [cloud up], поднимает колонну[column up],
      // поднимают виноград [grape up], шторки спускает
      closeLocks();
    }
    else if (start == 2)       //start game
    {
      mp3_set_serial(Serial);
      mp3_play(3); // start message
      delay(200);
      sendToSlave(lightConAddr, 0x11); // send random wind to lightController
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
    sendToSlave(lightConAddr, 0x12); // wind off
    delay(10);
    sendToSlave(motorConAddr, 0x12);  // turn on the lights
    send250ms(pressOUT);
    send250ms(gheraOUT);  // moves ghera to level 10
    level = 13;
    Serial.println("Go to level 13");
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
    send250ms(gheraOUT);  // ghera start speaking, shift Ghera forward - 'thunder' level
    level = 90;
    Serial.println("Gate Done, command 0x14 sent to motor and light , Go to level 90");
    sendToSlave(lightConAddr, 0x14); // send signal to light_controller >turnOnTheLights
  }
}

void Poseidon()
{
  if ((!digitalRead(poseiIN) || operGStates[poseidon]) && !passGStates[poseidon])
  { // signal from poseidon
    // posei > command via (i2c) to motor_controller activate falling column > players get first part thundi
    // shoud be skippable from master console
    passGStates[poseidon] = true;
    if (operGStates[poseidon]) send250ms(poseiOUT);
    sendToSlave(motorConAddr, 0x20); // Открываем тайник Посейдона, даем игрокам трезубец
    Serial.println("Poseidon Done");
  }
}

void Trident()
{
  if ((!digitalRead(triPin) || operGStates[trident]) && !passGStates[trident] && passGStates[poseidon])
  {
    sendToSlave(motorConAddr, 0x21); // Column Down ++++ EARTHQUAKE !!!!!!!!!!!!!!!!!!!!!!
    // START MP3 - FILE
    passGStates[trident] = true;
    Serial.println("Trident Done");
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
    // MP3 FILE rain
    passGStates[rain] = true;
    Serial.println("Rain Done");
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
    dioni1Timer = millis();
    // MP3 FILE
    Serial.println("Dionis-1 Done");
    delay(300);
  }
}

void Hercules()
{
  if ((!digitalRead(hercuIN) || operGStates[hercul]) && !passGStates[hercul])
  { // shoud be skippable from master console
    // hercu > players gets third part of thunders
    passGStates[hercul] = true;
    
    Serial.println("Hercules Done");
  }
}

void Narcis()
{
  if ((!digitalRead(narciIN) || operGStates[narcis]) && !passGStates[narcis])
  { // narci > pattern information for players
    // nothing happen - narcis only give players information
    // shoud be triggerable from master console
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
    // play mp3 thunder
    Serial.println("Thunder Done");
  }
}
void Afina1()
{
  if ((!digitalRead(afinaIN) || operGStates[afina1]) && !passGStates[afina1])
  { // afina  first signal > open afinaHD1  > players get knife
    
    if (operGStates[afina1]) send250ms(afinaOUT);
    passGStates[afina1] = true;
    Serial.println("Afina 1 part Done > small vault open");
    delay(300);
    // here afina gives players a key to open next vault
  }
}

void Afina2()
{
  if ((!digitalRead(afinaIN) || operGStates[afina2]) && !passGStates[afina2])
  { // afina second signal opens afinaHD2
    if (operGStates[afina2]) send250ms(afinaOUT);
    digitalWrite(afinaHD2, LOW);
    passGStates[afina2] = true;
    Serial.println("Afina 2 part Done > Large vault open");
    delay(200);
    // here afina gives players another part of the shield
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
  { // octop done > players get escu2
    // octopus has its own hideout, so nothing happens here
    // octopus gives players another part of the shield
    if (operGStates[octopus]) send250ms(octopOUT);
    passGStates[octopus] = true;
    Serial.println("Octopus Done");
  }
}

void Note()
{
  if ((!digitalRead(noteIN) || operGStates[note]) && !passGStates[note])
  {
    // note > players get escu3
    if (operGStates[note]) send250ms(noteOUT);
    
    // MP3 FILE
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
    delay(10);
    sendToSlave(lightConAddr, 0x31); // windBlow 10-15 secs
    // MP3 FILE
  }
}

void Ghera1()
{
  if ((!digitalRead(gheraIN) || operGStates[ghera1]) && !passGStates[ghera1])
  { //if all shields in ghera are in  place (gheraLevel = 40 ) gera speaks - open sealHD
    passGStates[ghera1] = true;
    //send250ms(musesOUT);
    if (operGStates[ghera1]) send250ms(gheraOUT);
    Serial.println("Ghera level 30 Done, SHIELDs Done");
    sendToSlave(lightConAddr, 0x30); // turn lights off while Ghera speaks
  }
}


void Fire()
{
  if ((!digitalRead(firePin) || operGStates[fire]) && !passGStates[fire])
  {
    // turner start
    sendToSlave(lightConAddr, 0x41); //turner start  - dim the room light 
    passGStates[fire] = true;
    Serial.println("Fire Done");
  }
}

void Flower1()
{
  if ((!digitalRead(flowrIN) || operGStates[flower1]) && !passGStates[flower1])
  { // first level of flower
    if (operGStates[flower1]) send250ms(flowrOUT);
    passGStates[flower1] = true;
    Serial.println("Flower-1 part Done");
    delay(300);
  }
}

void Flower2()
{
  if ((!digitalRead(flowrIN) || operGStates[flower2]) && !passGStates[flower2] && passGStates[flower1])
  { // second level of flower
    delay(50);
    sendToSlave(lightConAddr, 0x42); // turner off, light switch on
    if (operGStates[flower2]) send250ms(flowrOUT);
    passGStates[flower2] = true;
    Serial.println("Flower-2 part Done");
  }
}

void Dionis2()
{
  if ((!digitalRead(dioniIN) || operGStates[dionis2]) && !passGStates[dionis2])
  { // dionis(2) cold heart if all done
    if (operGStates[dionis2]) send250ms(dioniOUT);
    passGStates[dionis2] = true;
    Serial.println("Dionis-2 Done");
  }
}

void Arpha()
{
  if ((!digitalRead(musesIN) || operGStates[arpha]) && !passGStates[arpha])
  { //  signal from arpha received
    //    send250ms(musesOUT); // send signal to shut up the muses

    if (operGStates[arpha]) send250ms(musesOUT);

    digitalWrite(arphaHD, LOW);  // give players seal 3
    passGStates[arpha] = true;
    Serial.println("Arpha Done");

  }
}

void Ghera2()
{
  if (passGStates[flower2] && passGStates[arpha] && passGStates[dionis2])
  {
    if ((!digitalRead(gheraIN) || operGStates[ghera2]) && !passGStates[ghera2])
    {
      if (operGStates[ghera2]) send250ms(gheraOUT);
      passGStates[ghera2] = true;
      sealsDone = true;
      Serial.println("Seals Done");
      // if all seals are in ghera are in place  > ghera Level = 60   >>   sealsDone = true;
      // ((((  ghera speaks > open HD3 (key for underground) ))))
      sendToSlave(lightConAddr, 0x45); // turn lights off while Ghera speaks
    }
  }
}

void BigKey()
{
  if ((digitalRead(bigKeyIN) || operGStates[bigkey]) && !passGStates[bigkey])
  { //send to MotorController
    //if (operGStates[bigkey]) send250ms(bigKeyOUT);
    passGStates[bigkey] = true;
    Serial.println("BigKey Done");
  }
}

void Underground()
{
  underRFWait = !getUnderRFID();
  if ((!underRFWait || operGStates[under]) && !passGStates[under])
  {
    sendToSlave(motorConAddr, 0x51);
    passGStates[under] = true;
    send250ms(minotOUT);  // activate Minotavr
    underRFWait = false;
    Serial.println("Underground Opened");
  }
}

void Zodiak()
{
  if ((!digitalRead(zodiaIN) || operGStates[zodiak]) && !passGStates[zodiak] && passGStates[under])
  { // if zodia done > players get cryst1
    // shoud be skippable from master console
    // click lock 
    passGStates[zodiak] = true;
    Serial.println("Zodiak Done");
  }
}

void Minotavr()
{
  if ((!digitalRead(minotIN) || operGStates[minot]) && !passGStates[minot] && passGStates[under])
  { // if minotavr done > players get cryst2
    // shoud be skippable from master console
    // click lock 
    passGStates[minot] = true;
    Serial.println("Minot Done");
  }
}

void Gorgona()
{
  if ((!digitalRead(gorgoIN) || operGStates[gorgona]) && !passGStates[gorgona] && passGStates[under])
  { // if gorgona done > players get cryst3
    // shoud be skippable from master console
    // click lock 
    passGStates[gorgona] = true;
    Serial.println("Gorgona Done");
  }
}

void Crystals()
{
  if (passGStates[gorgona] && passGStates[minot] && passGStates[zodiak])
  {
    if (crystReciever)
    {
      Wire.requestFrom(lightConAddr, 1);
      byte lightUnsw = Wire.requestFrom(lightConAddr, 1, false);
      boolean crystSum = true;
      if (lightUnsw == 1)
      {
        byte unswer = Wire.read();
        Serial.println("CRYST STATES= " + String(unswer, BIN));
        for (int u = 0; u < 3; u++)
        {
          crystStates[u] = 1 & (unswer >> 2 * u);
          crystSum &= crystStates[u];
        }
        if (crystSum) passGStates[crystals] = true;
      }
      if (passGStates[crystals] || operGStates[crystals])
      {
        //level = 100;
        Serial.println("Crystals Done");
        //column up motor add!!!!!
        sendToSlave(motorConAddr, 0x99);
        send250ms(gheraOUT); // victory signal for Ghera
   //   sendToSlave(lightConAddr, 0x31); // turn lights off while Ghera speaks
        if (operGStates[crystals]) passGStates[crystals] = true;
      }
    }
    else
    {
     // if (!digitalRead(crystRecBut) || operGStates[crystals])
     // {
     //   //Open Cryst Reciever throw lightController
     //   sendToSlave(lightConAddr, 0x55);
     //   crystReciever = true;
     // }
    }
  }
}
