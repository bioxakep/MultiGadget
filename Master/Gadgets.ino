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
  if ((!digitalRead(balloIN) || operSkips[baloon]) && !gStates[baloon]) //signal from finished ballon received
  {
    if (operSkips[baloon]) 
    {
      send250ms(balloOUT);
      gStates[baloon] = true;
    }
    else playerGDone[baloon] = true;
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
  if (operSkips[baloon] && gStates[baloon])
  { // Выбери команды для выполнения при повторном скипе оператора
    sendToSlave(lightConAddr, 0x12); // wind off + lights on
    sendToSlave(motorConAddr, 0x12);  // turn on the lights
    send250ms(pressOUT);
    send250ms(gheraOUT);  // moves ghera to level 10
    
    send250ms(balloOUT);
    operSkips[baloon] = false;
  }
}

void Press()
{
  if ((!digitalRead(pressIN) || operSkips[presss]) && !gStates[presss]) //victory signal from press received
  {
    if (operSkips[presss]) 
    {
      send250ms(pressOUT);
      gStates[presss] = true;
    }
    else playerGDone[presss] = true;
    Serial.println("Press signal recieved. Press gave players the RFID key to the gate ");
    level = 14;
    Serial.println("Go to level 14");
  }
  if (operSkips[presss] && gStates[presss])
  {
    send250ms(pressOUT);
    operSkips[presss] = false;
  }
}

void Gate()
{
  gateRFWait = !getGateRFID();
  if ((!gateRFWait || operSkips[gate]) && !gStates[gate])
  {
    if (operSkips[gate]) gStates[gate] = true;
    else playerGDone[gate] = true;
    sendToSlave(motorConAddr, 0x14); // send signal to motor_controller >openGate
    sendToSlave(lightConAddr, 0x14); // send signal to light_controller >turnOnTheLights
    mp3_play(1);
    send250ms(gheraOUT);  // ghera start speaking, shift Ghera forward - 'thunder' level
    Serial.println("Gate Done, command 0x14 sent to motor and light , Go to level 90");
    level = 90;
  }
  if (operSkips[gate] && gStates[gate])
  {
    send250ms(gheraOUT);
    operSkips[gate] = false;
  }
}

void Poseidon()
{
  if ((!digitalRead(poseiIN) || operSkips[poseidon]) && !gStates[poseidon])
  { // signal from poseidon
    // posei > command via (i2c) to motor_controller activate falling column > players get first part thundi
    // shoud be skippable from master console
    
    sendToSlave(motorConAddr, 0x20); // Открываем тайник Посейдона, даем игрокам трезубец
    if (operSkips[poseidon]) 
    {
      gStates[poseidon] = true;
      send250ms(poseiOUT);
    }
    else playerGDone[poseidon] = true;
    mp3_play(5);
    delay(100);
    sendToSlave(motorConAddr, 0x20); // Открываем тайник Посейдона, даем игрокам трезубец -- REPEAT
    Serial.println("Poseidon Done");
  }
  if (operSkips[poseidon] && gStates[poseidon])
  {
    send250ms(poseiOUT);
    sendToSlave(motorConAddr, 0x20); // Открываем тайник Посейдона, даем игрокам трезубец -- REPEAT
    operSkips[poseidon] = false;
  }
}

void Trident()
{
  if ((!digitalRead(triPin) || operSkips[trident]) && !gStates[trident] && gStates[poseidon])
  {
    mp3_play(4); // START MP3 - FILE
    if (operSkips[trident]) gStates[poseidon] = true;
    else playerGDone[trident] = true;
    tridentTimer = millis();
    Serial.println("Trident started " + String(millis()));
  }
  if (operSkips[trident] && gStates[trident])
  {
    sendToSlave(motorConAddr, 0x21); //  или тоже по таймеру?
    operSkips[trident] = false;
  }
}

void Demetra()
{
  if ((!digitalRead(demetIN) || operSkips[demetra]) && !gStates[demetra])
  {
    if (operSkips[demetra]) 
    {
      gStates[demetra] = true;
      send250ms(demetOUT);
    }
    else playerGDone[demetra] = true;
    demetTimer = millis();
    Serial.println("Demetra 1 part Done");
  }
  if (operSkips[demetra] && gStates[demetra])
  {
    send250ms(demetOUT);
    operSkips[demetra] = false;
  }
}

void Rain()
{
  if ((!rainRFWait || operSkips[rain]) && !gStates[rain] && gStates[demetra])
  {
    if (operSkips[rain]) gStates[rain] = true;
    else playerGDone[rain] = true;
    sendToSlave(motorConAddr, 0x22); // send signal to motor_controller > grapeGrow
    mp3_play(3);     // MP3 FILE rain
    delay(100);
    sendToSlave(motorConAddr, 0x22); // send signal to motor_controller > grapeGrow   REPEAT !!!! just in case
    Serial.println("Rain Done");
  }
  if (operSkips[rain] && gStates[rain])
  {
    sendToSlave(motorConAddr, 0x22); // send signal to motor_controller > grapeGrow   REPEAT !!!! just in case
    operSkips[rain] = false;
  }
}

void Vine()
{
  if ((!digitalRead(vinemIN) || operSkips[vine]) && !gStates[vine]) // && gStates[demetra] && gStates[rain])
  {
    if (operSkips[vine]) 
    {
      gStates[vine] = true;
      send250ms(vinemOUT);
    }
    else playerGDone[vine] = true;
    send250ms(dioniOUT);  // inform Dionis the vine in done
    Serial.println("Vine Done");
  }
  if (operSkips[vine] && gStates[vine])
  {
    send250ms(vinemOUT);
    send250ms(dioniOUT);
    operSkips[vine] = false;
  }
}

void Dionis1()
{
  if ((!digitalRead(dioniIN) || operSkips[dionis1]) && !gStates[dionis1]) //&& gStates[vine])
  { // if players gives dionis empty bottle
    // he will tell them that he dont like empty bottles
    // if signal from full bottle is received >
    // dioniHD1 opens > players gets another thunder
    if (operSkips[dionis1]) 
    {
      send250ms(dioniOUT);
      gStates[dionis1] = true;
    }
    else playerGDone[dionis1] = true;
    // MP3 FILE
    digitalWrite(dioniHD1, HIGH); // open first dionis vault
    Serial.println("Dionis-1 Done");
    delay(300);  //rem on dec 3 to test - bad
    // delay while finish first command from dionis
  }
  if (operSkips[dionis1] && gStates[dionis1])
  {
    send250ms(dioniOUT);
    digitalWrite(dioniHD1, HIGH);
    operSkips[dionis1] = false;
  }
}

void Narcis()
{
  if ((!digitalRead(narciIN) || operSkips[narcis]) && !gStates[narcis])
  {
    if (operSkips[narcis]) 
    {
      send250ms(narciOUT);
      gStates[narcis] = true;
    }
    else playerGDone[narcis] = true;
    Serial.println("Narcis Done");
  }
  if (operSkips[narcis] && gStates[narcis])
  {
    send250ms(narciOUT);
    operSkips[narcis] = false;
  }
}

void Thunder()
{
  if ((!digitalRead(thundIN) || operSkips[thunder]) && !gStates[thunder])
  { // shoud be triggerable from master console
    // may add some extra storm effects thru light_controller
    //if all thunders are done >   // gheraLevel = 2 >  speaks > opend HD2 (shields)
    send250ms(gheraOUT);  // moves ghera to 'shields' level, ALWAYS OR BY OPERATOR?
    sendToSlave(lightConAddr, 0x23); // send signal to light_controller > GheraSpeaks, dim the light
    if (operSkips[thunder]) gStates[thunder] = true;
    else playerGDone[thunder] = true;
    thunderDone = true;
    mp3_play(88);     // play mp3 thunder
    Serial.println("Thunder Done");
  }
  if (operSkips[thunder] && gStates[thunder])
  {
    send250ms(gheraOUT);
    sendToSlave(lightConAddr, 0x23);
    operSkips[thunder] = false;
  }
}

void Afina1()
{
  if ( (!digitalRead(afinaIN) || operSkips[afina1] ) && !gStates[afina1])
  { // afina  first signal > open afinaHD1  > players get knife
    if (operSkips[afina1]) 
    {
      send250ms(afinaOUT);
      gStates[afina1] = true;
    }
    else playerGDone[afina1] = true;
    afinaTimer = millis();
    Serial.println("Afina 1 part Done > small vault open");
    delay(350);
    // here afina gives players a key to open next vault
  }
  if (operSkips[afina1] && gStates[afina1])
  {
    send250ms(afinaOUT);
    digitalWrite(afinaHD1, LOW); // Или тоже по таймеру?
    operSkips[afina1] = false;
  }
}

void Afina2()
{
  if ((!digitalRead(afinaIN) || operSkips[afina2]) && !gStates[afina2]) //(!digitalRead(afinaIN) ||
  { // afina second signal opens afinaHD2
    //if (operSkips[afina2]) send250ms(afinaOUT);
    if (operSkips[afina2]) gStates[afina2] = true;
    else playerGDone[afina2] = true;
    digitalWrite(afinaHD2, LOW);
    Serial.println("Afina 2 part Done > Large vault open");
    delay(200);
  }
  if (operSkips[afina2] && gStates[afina2])
  {
    digitalWrite(afinaHD2, LOW);
    operSkips[afina2] = false;
  }
}

void TimeG()
{
  if ((!digitalRead(timeIN) || operSkips[Time]) && !gStates[Time])
  {
    if (operSkips[Time]) 
    {
      send250ms(timeOUT);
      gStates[Time] = true;
    }
    else playerGDone[Time] = true;
    Serial.println("Time part Done");
  }
  if (operSkips[Time] && gStates[Time])
  {
    send250ms(timeOUT);
    operSkips[Time] = false;
  }
}

void Octopus()
{
  if ((!digitalRead(octopIN) || operSkips[octopus]) && !gStates[octopus]) // && gStates[Time])
  {
    if (operSkips[octopus]) 
    {
      send250ms(octopOUT);
      gStates[octopus] = true;
    }
    else playerGDone[octopus] = true;
    Serial.println("Octopus Done");
  }
  
  if(operSkips[octopus] && gStates[octopus])
  {
    send250ms(octopOUT);
    operSkips[octopus] = false;
  }
}

void Note()
{
  if ((!digitalRead(noteIN) || operSkips[note]) && !gStates[note])
  {
    // note > players get wind amulet
    if (operSkips[note]) 
    {
      send250ms(noteOUT);
      gStates[note] = true;
    }
    else playerGDone[note] = true;
    digitalWrite(noteHD, LOW);
    Serial.println("Note Done");
  }
  if (operSkips[note] && gStates[note])
  {
    send250ms(noteOUT);
    digitalWrite(noteHD, LOW);
    operSkips[note] = false;
  }
}

void Wind()
{
  if ((!windRFWait || operSkips[wind]) && !gStates[wind]) //&& gStates[note]) //disc
  { //Из тайника игроки получают рфид ветра.и когда вставляют его в приемник рфида ветра то включаем ветер на 10-15 секунд,
    //мр3 файл и спускаем облакоИз тайника игроки получают рфид ветра.и когда вставляют его в приемник рфида
    //ветра то включаем ветер на 10-15 секунд, мр3 файл и спускаем облако
    if (operSkips[wind]) gStates[wind]= true;
    else playerGDone[wind] = true;
    Serial.println("Wind RFID Recieved");
    sendToSlave(motorConAddr, 0x31); // CloudDown - отдать 3 часть щита
    delay(50);
    sendToSlave(lightConAddr, 0x31); // windBlow 10-15 secs
    mp3_play(2);    // Wind MP3 FILE
  }
  if(operSkips[wind] && gStates[wind])
  {
    sendToSlave(motorConAddr, 0x31); // CloudDown - отдать 3 часть щита
    delay(50);
    sendToSlave(lightConAddr, 0x31); // windBlow 10-15 secs
    operSkips[wind] = false;
  }
}

void Ghera1() // SHIELDS done
{
  if ((!digitalRead(gheraIN) || operSkips[ghera1]) && !gStates[ghera1])
  { //if all shields in ghera are in  place (gheraLevel = 40 ) gera speaks - open sealHD
    mp3_play(12);
    if (operSkips[ghera1]) 
    {
      send250ms(gheraOUT);
      gStates[ghera1] = true;
    }
    else playerGDone[ghera1] = true;
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
    playerGDone[afina2] = true;
    Serial.println("Ghera level 30 Done, SHIELDs Done");
    sendToSlave(lightConAddr, 0x30); // turn lights off while Ghera speaks
  }
  // Не стал писать повторное срабатывание по хинту, если надо напишу.
}

void Fire()
{
  if ((!digitalRead(firePin) || operSkips[fire]) && !gStates[fire])
  {
    if (operSkips[fire]) gStates[fire] = true;
    else playerGDone[fire] = true;
    mp3_play(77);
    sendToSlave(lightConAddr, 0x41); //turner start  - dim the room light
    Serial.println("Fire Done, turner start...");
  }
  if (operSkips[fire] && gStates[fire])
  {
    sendToSlave(lightConAddr, 0x41); //turner start  - dim the room light
    operSkips[fire] = false;
  }
}

void Flower1()  // first level of flower
{
  if ((!digitalRead(flowrIN) || operSkips[flower1]) && !gStates[flower1])
  {
    if (operSkips[flower1]) 
    {
      send250ms(flowrOUT);
      gStates[flower1] = true;
    }
    else playerGDone[flower1] = true;
    mp3_play(6);
    Serial.println("Flower-1 Blue part Done");
    delay(300);
  }
  if (operSkips[flower1] && gStates[flower1])
  {
    send250ms(flowrOUT);
    operSkips[flower1] = false;
  }
}

void Flower2()    // second level of flower
{
  if ((!digitalRead(flowrIN) || operSkips[flower2]) && !gStates[flower2] && gStates[flower1])
  {
    if (operSkips[flower2]) 
    {
      send250ms(flowrOUT);
      gStates[flower2] = true;
    }
    else playerGDone[flower2] = true;
    mp3_play(7);
    delay(50);
    sendToSlave(lightConAddr, 0x42); // turner off, light switch
    Serial.println("Flower-2 RED part Done");
    flowerTimer = millis();
  }
  if (operSkips[flower2] && gStates[flower2])
  { // Выбери что нужно повторять оператором
    sendToSlave(lightConAddr, 0x42);
    send250ms(flowrOUT);
    operSkips[flower2] = false;
  }
}


void Dionis2()  // dionis(2) cold heart if all done
{
  if ((!digitalRead(dioniIN) || operSkips[dionis2]) && !gStates[dionis2] && gStates[ghera1])
  {
    if (operSkips[dionis2]) 
    {
      send250ms(dioniOUT);
      gStates[dionis2] = true;
    }
    else playerGDone[dionis2] = true;
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
  if (gStates[flower2] && gStates[dionis2])
  {
    if ((!digitalRead(gheraIN) || operSkips[ghera2]) && !gStates[ghera2])
    {
      if (operSkips[ghera2]) 
      {
        send250ms(gheraOUT);
        gStates[ghera2] = true;
      }
      else playerGDone[ghera2] = true;
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
  if (operSkips[ghera2] && gStates[ghera2])
  {
    sendToSlave(lightConAddr, 0x45); // turn lights off while Ghera speaks
    send250ms(gheraOUT);
    operSkips[ghera2] = false;
  }
}

void BigKey()
{
  if ((digitalRead(bigKeyIN) || operSkips[bigkey]) && !gStates[bigkey])
  {
    if (operSkips[bigkey]) gStates[bigkey] = true;
    else playerGDone[bigkey] = true;
    sendToSlave(motorConAddr, 0x51);
    mp3_play(8);
    Serial.println("BigKey Done");
  }
  if (operSkips[bigkey] && gStates[bigkey])
  {
    sendToSlave(motorConAddr, 0x51);
    operSkips[bigkey] = false;
  }
}

void Underground()
{
  underRFWait = !getUnderRFID();
  if ((!underRFWait || operSkips[under]) && !gStates[under])
  {
    if (operSkips[under]) gStates[under] = true;
    else playerGDone[under] = true;
    sendToSlave(motorConAddr, 0x53);
    mp3_play(9);
    send250ms(minotOUT);  // activate Minotavr
    send250ms(narciOUT);  // shift Narcis to show underground map
    underRFWait = false;
    Serial.println("Underground Opened");
  }
  if (operSkips[under] && gStates[under])
  { // Выбери что нужно повторять оператором
    sendToSlave(motorConAddr, 0x53);
    send250ms(minotOUT);  // activate Minotavr
    send250ms(narciOUT);  // shift Narcis to show underground map
    operSkips[under] = false;
  }
}

void Minotavr()
{
  if ((!digitalRead(minotIN) || operSkips[minot]) && !gStates[minot] && gStates[under])
  {
    if (operSkips[minot]) gStates[minot] = true;
    else playerGDone[minot] = true;
    Serial.println("Minot Done");
    minotTimer = millis();
  }
  if (operSkips[minot] && gStates[minot])
  {
    send250ms(minotHD); //220
    operSkips[minot] = false;
  }
}

void Gorgona()
{
  if ((!digitalRead(gorgoIN) || operSkips[gorgona]) && !gStates[gorgona] && gStates[under])
  {
    if (operSkips[gorgona]) gStates[gorgona] = true;
    else playerGDone[gorgona] = true;
    send250ms(gorgoHD); //220
    Serial.println("Gorgona Done");
  }
  if (operSkips[gorgona] && gStates[gorgona])
  {
    send250ms(gorgoHD); //220
    operSkips[gorgona] = false;
  }
}

void Crystals()
{
  if (gStates[gorgona] && gStates[minot])
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
      if (crystCheck) gStates[crystals] = true;
    }
    if (cristSum != cristCount)
    {
      if (cristSum > 0) mp3_play(10);
      cristCount = cristSum;
    }
    if (gStates[crystals] || operSkips[crystals])
    {
      // mp3_play(10); //each one (1-2)
      delay(100);
      mp3_play(11); // then all 3

      //level = 100;
      Serial.println("Crystals Done");
      sendToSlave(motorConAddr, 0x99);  // game over = stop the underground music
      send250ms(gheraOUT); // victory signal for Ghera
      send250ms(octopOUT); // 250 ms to Octopus when crystals done.
      // turn lights off while Ghera speaks
      if (operSkips[crystals])
      {
        playerGDone[crystals] = true;
        sendToSlave(lightConAddr, 0x55); // inform light_controller than we skipping crystals
      }
    }
  }
}

void Hercules()
{
  if ((!digitalRead(hercuIN) || operSkips[hercul]) && !gStates[hercul])
  {
    // hercu > players gets third part of thunders
    if (operSkips[hercul]) gStates[hercul] = true;
    else playerGDone[hercul] = true;
    mp3_play(66);
    digitalWrite(hercuHD,LOW); // open Hercules HD
    Serial.println("Hercules Done");
  }
  if (operSkips[hercul] && gStates[hercul])
  {
    digitalWrite(hercuHD,LOW); // open Hercules HD
    operSkips[hercul] = false;
  }
}

void Arpha()   //  signal from arpha received
{
  if ((!digitalRead(musesIN) || operSkips[arpha]) && !gStates[arpha])
  {
    send250ms(musesOUT); // send signal to PLAY 998 AND shut up the muses
    if (operSkips[arpha]) 
    {
      send250ms(musesOUT);
      gStates[arpha] = true;
    }
    else playerGDone[arpha] = true;
    arphaTimer = millis();
    Serial.println("Arpha Done + signal to muses sent");
  }
  if (operSkips[arpha] && gStates[arpha])
  {
    send250ms(musesOUT);
    digitalWrite(arphaHD, LOW);  // give players seal 3  // OR TIMER
    operSkips[arpha] = false;
  }
}

void Zodiak()
{
  if ((!digitalRead(zodiaIN) || operSkips[zodiak]) && !gStates[zodiak])
  {
    if (operSkips[zodiak]) gStates[zodiak] = true;
    else playerGDone[zodiak] = true;
    sendToSlave(motorConAddr, 0x55);
    Serial.println("Zodiak Done");
    send250ms(zodiaHD); //220
  }
  if (operSkips[zodiak] && gStates[zodiak])
  {
    send250ms(zodiaHD); //220
    sendToSlave(motorConAddr, 0x55); // IF NEED
    operSkips[zodiak] = false;
  }
}

void Bonus()
{
  if ((!digitalRead(octopIN) || operSkips[bonus]) && !gStates[bonus])
  {
    if (operSkips[bonus]) 
    {
      send250ms(octopOUT);
      gStates[bonus] = true;
    }
    else playerGDone[bonus] = true;
    Serial.println("Octopus-2 Done");
  }
  if(operSkips[bonus] && gStates[bonus])
  {
    send250ms(octopOUT);
    operSkips[bonus] = false;
  }
}
