void loop()
{
  printCount();


  if (gameWin == false) Shaking(1);
  
  // ================================================ LEVEL 0 ================================================ //
  if (level == 0)
  {
    byte bufercnt = Wire.requestFrom(peopleCAddress, 1, false); //Request from operator params
    boolean p1State = false;
    boolean p2State = false;
    if (bufercnt == 1)
    {
      byte playerStates = Wire.read();
      p1State = (playerStates >> 2) & 0x01;
      p2State = (playerStates >> 6) & 0x01;

      //  Serial.println("player1State = " + String(p1State));
      //  Serial.println("player2State = " + String(p2State));

      if (p1State ) {
        digitalWrite (handlockB, HIGH);
        if ( fisrtSeatReady == false ) {
          fisrtSeatReady = true;
          Serial.println("#S|UNLOCK1|[]#");
        }
      }

      if (p2State) {
        digitalWrite (handlockA, HIGH);
        if ( fisrtSeatReady == false ) {
          fisrtSeatReady = true;
          Serial.println("#S|UNLOCK1|[]#");
        }
      }

    }

    if ((p1State && p2State) || skipLevel)
    {
      Serial.println("#S|UNLOCK2|[]#");
      delay(3000);
      digitalWrite( handlockA, HIGH);
      Serial.println(" handlockA, HIGH ");
      delay(2000);
      digitalWrite( handlockB, HIGH);
      Serial.println(" handlockB, HIGH ");
      delay(300);
      Serial.println("Both players are free");
      Serial.println("waiting for seats to fold...");

      moveAdmin(); // next level on Amdin 

      // dispA1();   // start the upper screen
      dispB1();   // start the lower screen
      level = 10;
      if (skipLevel) {
        skipLevel = false;
        delay(300);
      }
    }
  }
  // ================================================ LEVEL 10 ================================================ //
  else if (level == 10) // Wait for both seats folded.
  {
    boolean seatAFolded = digitalRead(seatA);
    boolean seatBFolded = digitalRead(seatB);
    if (seatAFolded) digitalWrite(seatAfd, HIGH);
    else digitalWrite(seatAfd, LOW);
    if (seatBFolded) digitalWrite(seatBfd, HIGH);
    else digitalWrite(seatBfd, LOW);

    if ((seatAFolded && seatBFolded) || skipLevel)
    {
      digitalWrite(seatAfd, HIGH);
      delay(300);
      digitalWrite(seatBfd, HIGH);
      delay(300);
      digitalWrite(FOCUS, HIGH);  // turn focus lights off
      delay(300);

      moveAdmin(); // next level on Amdin 

      Serial.println("Both seats are folded");
      Serial.println("Wait for STOP HANDLE...");
      //dispA1();   // start the upper screen
      dispB1();   // start the lower screen
      level = 20;
      if (skipLevel) {
        skipLevel = false;
        delay(300);
      }
    }
    // ========== SHAKE IT =============
    if (!digitalRead(StopHU) && !warnPlayed) {
      warnPlayed = true;
      Serial.println("#S|FOLDSEATS|[]#");
      pnDN(300);
    }
  }
  // ================================================ LEVEL 20 ================================================ //
  else if (level == 20) // Ждем остановки рычагом
  {
    huStates[0] = readButt(huStates[1], StopHU);
    if (skipLevel || (!huStates[0] && huStates[1]) || warnPlayed)
    {
      Serial.println(" " );
      mp3_play(1111);  // stop sound from DF player
      Serial.println(" " );
      pnUP(1200);
      delay(100);
      pnDN(300);
      delay(100);
      pnDN(300);

      moveAdmin(); // next level on Amdin 

      digitalWrite(stopped, HIGH);
      delay(300);
      Serial.println("Video leaks");
      Serial.println("#S|MAIN3|[]#");       /// STOP
      dispA1();   // start the upper screen
      dispB1();   // start the lower screen
      delay(5000);
      Serial.println("#S|TRACK3|[]#"); // audio track

      Serial.println("capsule stopped, heavy shake and damage to the hull");
      Serial.println("LEAKS start");

      // clock can show random nubers changing quickly (2 per second)

      Goleaks();
      digitalWrite(HIDE1, LOW); // rel 1 hide 3 HIGH = CLOSED AND NO LIGHT
      Serial.println("release HIDE1 (solder)");
      //Останавливаем кабину (пневмо-команды)
      level = 30;

      digitalWrite(blu24v ,    HIGH);  // OFF rel 15  blue 24v light
      delay(300);
      digitalWrite(red24v,     LOW);   // ON  rel 16  red 24v light
      delay(300);
      digitalWrite(bluTop,     LOW);   // OFF
      delay(300);
      digitalWrite(redTop,     HIGH);  // ON
      delay(300);

      digitalWrite(stopped, HIGH);
      if (skipLevel) {
        skipLevel = false;
        delay(300);
      }
    }
    huStates[1] = huStates[0];
  }
  // ================================================ LEVEL 30 ================================================ //
  else if (level == 30) //Wait for leaks ok and VAC game started//
  {
    //    boolean leakOk = !readButt(HIGH, leakSig);
    //    if (leakOk || skipLevel)
    if (!digitalRead(leakSig) || skipLevel)
    {
      Serial.println("#S|MAIN4|[]#");
      dispA1();   // start the upper screen
      dispB1();   // start the lower screen
      Serial.println("#S|TRACK4|[]#"); // audio track

      moveAdmin(); // next level on Amdin 

      Serial.println("LEAKS game finished, signal to turn off the LEAKS");
      digitalWrite(toLeak, HIGH);
      delay(500);
      Serial.println("Master LEAKS valve closed");
      digitalWrite(LeakMas, HIGH);  // rel 11 HIGH - CLOSED - OFF
      delay(500);

      digitalWrite(blu24v ,    LOW);  // ON rel 15  blue 24v light
      delay(300);
      digitalWrite(red24v,     HIGH);   // OFF  rel 16  red 24v light
      delay(300);
      digitalWrite(bluTop,     HIGH);   // ON  ????
      delay(300);
      digitalWrite(redTop,     LOW);  // OFF ????
      delay(300);

      Serial.println(" Start VAC GAME ... " );
      digitalWrite(vac_Out, HIGH);
      delay(500);
      Serial.println(" ... free HIDE2 radio, time = time-15min " );
      digitalWrite(HIDE2, LOW);
      elapsedTime -= 15;

      Serial.println(" playing VAC game... " );
      Serial.println("    " );

      if (skipLevel)
      {
        Serial.println("Passed by operator");
        skipLevel = false;
        delay(300);
      }
      else Serial.println("Won by player");
      level = 40;
    }
  }
  // ================================================ LEVEL 40 ================================================ //
  else if (level == 40)
  {
    vacStates[0] = readButt(vacStates[1], VacSig);
    if ((!vacStates[0] && vacStates[1]) || skipLevel)
    {
      if (skipLevel) {
        skipLevel = false;
        delay(300);
      }
      //   digitalWrite(vac_Out, LOW);    // forced finish for VAC game
      Serial.println("#S|ANTENNA|[]#");

      moveAdmin(); // next level on Amdin 

      level = 50;
      dispA1();   // start the upper screen
      dispB1();   // start the lower screen
      Serial.println("VAC game done, coordinates sent");
      // Serial.println("video on A (greetings from team)");
      // Serial.println("release HIDE3 (extension) players should free the cpasule from lines");
      digitalWrite(HIDE3, LOW);
      Serial.println("********************************************************************");
      delay(500);
    }
    vacStates[1] = vacStates[0];
  }
  // ================================================ LEVEL 50 ================================================ //
  else if (level == 50) //Roof level
  {
    roofStates[0] = readButt(roofStates[1], roof);
    if ((!roofStates[0] && roofStates[1]) || skipLevel)
    {
      Serial.println("#S|DETACHED|[]#");
      delay(300);
      Serial.println("#S|TRACK5|[]#"); // audio track

      moveAdmin(); // next level on Amdin 

      delay(8000);
      //  Serial.println("Capsule unhooked from lines");
      pnUP(800);
      pnDN(500);
      //   Serial.println("starting to go up");
      delay(2000);
      digitalWrite(bluTop,     LOW);   // OFF
      //     Serial.println("start Octopus attack");
      //     Serial.println("Heavy shake and red lights");
      pnDN(500);
      digitalWrite(blu24v ,    HIGH);  // OFF rel 15  blue 24v light
      delay(300);

      dispA1();   // start the upper screen
      dispB1();   // start the lower screen
      pnUP(900);
      delay(300);
      pnDN(700);
      digitalWrite(redTop,     HIGH);  // ON
      delay(300);
      pnUP(800);
      Serial.println("activate charge game");
      digitalWrite(red24v,     LOW);   // ON  rel 16  red 24v light
      delay(300);


      digitalWrite(toCharA, HIGH);
      Serial.println("#S|MAIN5|[]#"); // Octopus Video start
      delay(200);
      pnDN(700);
      digitalWrite(toCharA, LOW);
      delay(300);
      digitalWrite(toCharB, LOW);
      level = 60;
      sendToSlave(peopleCAddress, 0x02); // HALL Game activate on PeopleController
      delay(500);
      pnUP(1200);

      if (skipLevel) {
        skipLevel = false;
        delay(300);
      }
      //
      // lights
      // damages to the electric system
      // red lights
    }
    roofStates[1] = roofStates[0];
  }
  // ================================================ LEVEL 60 ================================================ //
  else if (level == 60) // Octopus attack
  {
    //======= SHAKE IT ===============
    ///stabilize on/of *******************  shake a little bit  **************************
    if (!digitalRead(CharSigB) && !STAstate) {
      if (!STAstate) {
        pnUP(300);
        STAstate = true;
      }
    }
    if (digitalRead(CharSigB) && STAstate) {
      if (STAstate)  {
        pnDN(300);
        STAstate = false;
      }
    }

    if (!digitalRead(CharSigC)) {
      digitalWrite(FANS, HIGH);  ///FAN on/off
    } else {
      digitalWrite(FANS, LOW);
    }
    //======= SHAKE IT ===============
    CharSigAStates[0] = readButt(CharSigAStates[1], CharSigA);
    if ((!CharSigAStates[0] && CharSigAStates[1]) || skipLevel)
    {
      Serial.println(" "); //clean
      Serial.println("#S|MAIN6|[]#");
      sendToSlave(peopleCAddress, 0x03); // Send to processing dark screen mode

      moveAdmin(); // next level on Amdin

      delay(300);
      Serial.println(" ");
      Serial.println("#S|TRACK6|[]#"); // audio track
      delay(300);

      Serial.println("************************************************************");
      Serial.println("all energy transfered to external armor");
      Serial.println("lights strikes");
      Serial.println("and blackout...");
      Serial.println("CHARGE game done, turn lights off (all display dark)");
      dispA1();   // start the upper screen
      dispB1();   // start the lower screen
      digitalWrite(sw12v  ,    LOW); //OFF
      delay(300);
      Serial.println("#S|VOICE6|[]#"); // audio track
      digitalWrite(  seatAfd,  LOW);   // turn off seat A folded LED
      delay(300);
      digitalWrite(  seatBfd,  LOW);   // turn off seat B folded LED
      delay(300);
      digitalWrite(  stopped,  LOW);   // turn off stopped LED
      delay(300);
      digitalWrite(HDlight,   HIGH); //OFF  TURN OFF AL HIDES
      delay(300);
      digitalWrite(FANS,       HIGH); //OFF
      delay(300);
      digitalWrite(FOCUS,      HIGH);//OFF
      delay(300);
      digitalWrite(blu24v ,    HIGH);//OFF
      delay(300);
      digitalWrite(red24v,     HIGH);//OFF
      delay(300);
      digitalWrite(bluTop,     LOW); //OFF
      delay(300);
      digitalWrite(redTop,     LOW); //OFF
      delay(300);

      drawBlack();
      Serial.println("release HIDE4 (IR charger)");
      digitalWrite(HIDE4, LOW);
      Serial.println("CHARGING game started / local") ;
      Serial.println("********************************************** ");
      level = 70;

      delay(1000);  // to avoid continuos skip to next level
      if (skipLevel)
      {
        digitalWrite(toCharB, HIGH);
        skipLevel = false;
        delay(500);
        digitalWrite(toCharB, LOW);
      }
    }
    CharSigAStates[1] = CharSigAStates[0];
    //  wait for tubes >> open HIDE - Defibrilator !!!skip first hidemas second charge
    if (deFibroClosed)
    {
      byte bufercnt = Wire.requestFrom(peopleCAddress, 1, false);
      if (bufercnt == 1)
      {
        //      Serial.print("HALL DATA RECEIVED::");
        byte hallDone = Wire.read();
        //       Serial.println(String(hallDone));
        if (hallDone == 0x01)
        {
          Serial.println("Defibro opened");
          mp3_play(1000);
          digitalWrite(HIDEMAS, LOW);
          deFibroClosed = false;
        }
      }
    }
  }
  // ================================================ LEVEL 70 ================================================ //
  else if (level == 70)
  {
    charging();

    if (chargeGame || skipLevel)
    {
      Serial.println("energy recharged,  ");

      moveAdmin(); // next level on Amdin 

      digitalWrite(toCharA, HIGH); //signal to charger to goto Auto mode// finish
      delay(500);
      digitalWrite(toCharA, LOW);
      Serial.println("CHARGE game done, turn lights on");

      Serial.println("#S|MAIN7|[]#");
      dispA1();   // start the upper screen
      Serial.println("#S|VOICE7|[]#");
      dispB1();   // start the lower screen
      Serial.println("#S|TRACK7|[]#"); // audio track
      digitalWrite(sw12v  ,    HIGH);  // HIGH = ON
      digitalWrite(HDlight,    LOW);  //  ON,     ***** turn all HIDEs back ON
      digitalWrite(FANS,       HIGH);  // HIGH = ON
      digitalWrite(FOCUS,      LOW);   // LOW = ON, small light
      digitalWrite(blu24v ,    LOW);   // ON
      digitalWrite(red24v,     HIGH);  // OFF
      digitalWrite(bluTop,     HIGH);  // ON
      digitalWrite(redTop,     LOW);   // OFF
      Serial.println(" Start video  and the GoUp game");
      delay(15000);
      sendToSlave(peopleCAddress, 0x04); //Send to processing start upGame.
      level = 80;
      drawBlue();

      Serial.println("#S|DANCE|[]#");
      delay(1000); // avoid auto-move
      if (skipLevel)
      {
        chargePower();
        skipLevel = false;
        delay(500);
      }
    }
  }
  // ================================================ LEVEL 80 ================================================ //
  else if (level == 80) //upGame wait
  {
    byte bufercnt = Wire.requestFrom(peopleCAddress, 1, false);
    if (bufercnt == 1)
    {
      byte upGameDone = Wire.read();
 //     Serial.print("upGameDone = ");
 //     Serial.println(upGameDone, HEX);
      if (upGameDone == 0x01 || skipLevel)
      {
        level = 90;
  
        moveAdmin(); // next level on Amdin 
        
        gameWin = true;
        skipLevel = false;
        //Video, Sound   ******************************************************************* VICTORY !!!!!!!!!!!!!
        sendToSlave(peopleCAddress, 0x05); //Send to people and to processing win splash start.
      }
    }
  }
  // ================================================ LEVEL 90 ================================================ //
  else if (level == 90) //Wait for player going out. Win or Loose.
  {
    //Win Game, restart...
    Serial.println(" " );
    Serial.println("WIN GAME");
    dispA1();   // the upper screen
    level = 100;
  }
  // ================================================ LEVEL 100 ================================================ //
  else if (level == 100) //Wait for player going out. Win or Loose.
  {
    Serial.println(" ");  // video only
    Serial.println("#S|VICTORYV|[]#");  // video only
   Serial.println("  ");
   delay(300);
  mp3_play(666);
   Serial.println("  ");
   delay(300);
    digitalWrite(blu24v ,    LOW);
      pnDN(500);
    delay(400);
    digitalWrite(red24v,     HIGH);
      pnUP(800);
    delay(200);
      pnDN(500);
    digitalWrite(bluTop,     HIGH);
    delay(300);
      pnUP(900);
    digitalWrite(redTop,     HIGH);
    delay(150);
      pnDN(350);
    digitalWrite(DOOR, HIGH);

      pnUP(800);
      pnDN(500);
      delay(2000);
//      pnDN(500);
//      pnUP(900);
//      delay(300);
//      pnDN(700);
//      pnUP(800);
//      delay(800);
//      pnDN(700);
//      pnUP(1200);



    
    level = 101;
  }

  remoteState[0] = readButt(remoteState[1], REMOTE);
  if (!remoteState[0] && remoteState[1])
  {
    if (level == 60 && deFibroClosed)
    {
      deFibroClosed = false;
      digitalWrite(HIDEMAS, LOW);
    }
    else skipLevel = true;
    Serial.println("*****************************");
    Serial.println("** Operator skip / Level = " + String(level));
    Serial.println("*****************************");
  }
  remoteState[1] = remoteState[0];
}
//eof_loop
