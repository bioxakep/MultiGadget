void loop() {       // 2 - VIDEO HELPS
  timer.run();

  if ( !digitalRead(emerA) || !digitalRead(emerB) ) {
    lastlevel = level;
    emergency();
  }
  
  if (digitalRead(emerA) == LOW){
    Serial.println(" emergency A " );  
    delay(900);
  }
  if (digitalRead(emerB) == LOW) {
    Serial.println(" emergency B " );  
    delay(900);
  }

if (level == 0) {  //wait for UNO 

    // start the audio
    // wait for the video 
    // move pheumatics
    Serial.println("#S|MAIN_02|[]#"); // going down
    // video1 = 2;

    Serial.print(" Waiting for UNO .. (or remote)" );  
    leerRemote();
    espera();
    
    // release the hand locks thru UNO
    Serial.println(" Both players are free .. " );  
    digitalWrite( handlockA, LOW);  // rel 12
    digitalWrite( handlockB, LOW);  // rel 13

    // mp3 fono
    level = 50;      
    Serial.print(" Waiting for Seats to be folded ... (or remote) on level 50" );  
    checkInputs();
    espera(); 
}
 //eof_level_0

if (level == 50) { //uno ended waiting for seats (folded)
    if (digitalRead(seatA)==LOW) {
      digitalWrite( seatAfd,HIGH); 
    } else {digitalWrite( seatAfd,LOW);}
    
    if (digitalRead(seatB)==LOW){
      digitalWrite( seatBfd,HIGH); 
    } else {digitalWrite( seatBfd,LOW);}

    if ((digitalRead(seatA)==LOW && digitalRead(seatB)==LOW) || digitalRead(remote) == LOW )  {
      level = 60;
      digitalWrite( seatAfd,HIGH); 
      digitalWrite( seatBfd,HIGH); 
      Serial.println(" Both seats are folded waiting for STOP HANDLE  .. " );  
      Serial.println(" Waiting for STOP signal ...(or remote) on level 60" );  
      checkInputs();
      espera();
    }
}
 //eof_level_50

if (level == 60) { // waiting STOP signal
    if ( digitalRead(StopHU) == LOW  || digitalRead(remote) == LOW ) {
     Serial.println(" lift stopped... " );  
     digitalWrite(stopped, HIGH);   // BRW
     level =  100;
     checkInputs();
     espera(); 
    }
}   
 //eof_level_60

 if (level == 100) {   // lift stopped 
    Serial.println(" Hull damaged, starting lo leak...(GoLeaks) " );  
    Serial.println("#S|MAIN_03|[]#"); 
    // clock goes crazy
    Goleaks();
    level = 110;
}
 // eof_level_100

if (level == 110) {   // leaks started, psssssss ....
   if ( digitalRead(leakSig) == LOW || digitalRead(remote) == LOW ) {
    Serial.print(" leaks are fixed..." );  
    if (digitalRead(remote)==LOW ) {  // if manual override
      digitalWrite(toLeak,HIGH);
      delay(200);
      digitalWrite(toLeak,LOW);
    }
    digitalWrite(LeakMas, HIGH);  // rel 11 CLOSED - OFF
    level = 200;
    checkInputs();
    espera(); 
   }
 }
 //eof_level_110 (while leaks game is running)


if (level == 200)  {  //if leaks are fixed
// restore the lights, the video
// open HIDE   radio
   Serial.println(" ... free hide radio, start VAC GAME " );  
   digitalWrite(  hide2,    LOW);  
   digitalWrite(  vac_Out, HIGH);    // BLK
   delay(200);
   digitalWrite(  vac_Out,  LOW);    // BLK
   level = 300;
}
 //eof_level_200

if( level == 300) {   //wait for VAC 

 if (digitalRead(VacSig)==LOW || digitalRead(remote)==LOW){
    if (digitalRead(remote)==LOW ) {  // if manual override
      digitalWrite(vac_Out,HIGH);
      delay(200);
      digitalWrite(vac_Out,LOW);
    }
    //free extesion hide
    // coordenadas enviadas
    // video 
    // audio
    // se mueve hacia arriba - accenso
    level = 400;
    Serial.println(" VAC GAME done" );  
    Serial.println(" display viedo message on Display  " );  
    Serial.println(" " );  
    Serial.println(" Now play the roof to release from the lines " );  
    espera(); 
 }
}
 //eof_level_300

if (level == 400)  {  //after vac game
  if (digitalRead(roof) == LOW || digitalRead(remote)==LOW){
   Serial.println(" released from the lines ... " );  
   Serial.println("#S|MAIN_04|[]#"); 
   level = 500;

   checkInputs();
   espera();
  }
}
 //eof_level_400

if (level == 500) {   // octopus attack
// lights
// damages to the electric system
// red lights
  digitalWrite(   toCharA, HIGH);   // activate Charger 
  delay(200);
  digitalWrite(   toCharA, LOW);   // 

//  wait for tubes >> open HIDE - Defibrilator
   level = 510;
}
 //eof_level_500

if (level == 510)  {  //after vac game
  if ((digitalRead(CharSigA) == LOW && digitalRead(CharSigB) == LOW) || digitalRead(remote)==LOW){   // both signal recieved
    Serial.println(" Charger finished ... " );  
     if (digitalRead(remote) == LOW) {
           digitalWrite(   toCharA, HIGH);   // finish the Charger 
           delay(1200);
           digitalWrite(   toCharA, LOW);   // 
     }
    // shock the octopus
    // light blinking
    // everything off  
    // hide charger open 
    // voice record to guide to the charger
    // video1 = 4;
    level = 600;
    espera(); 
  }

  if (digitalRead(CharSigA) == LOW) { //turn off the lights
  }

  if (digitalRead(CharSigB) == LOW) { //turn off the vents
  }

  if ( tubesPass == false ) //  check the heart rate sensor - magnet readed by hall sensors (x6 / 2)
     {
     chkTubes();
     } 
     else 
     {
     digitalWrite(hide4, LOW);  // defibrilator box
     delay(300);
     tubesPass == false;
     }
  
}
 //eof_level_510
 
if (level == 600 ) {  //charger game
  if (chargeGame == false ) {
      charging();
     } else {
     level = 700; 
           for (int w=0; w < 6; w++) { // turn off IR_LEDs
                digitalWrite(irled[w], LOW);
           }
     //minimum light like at the start
     Serial.println("#S|MAIN_06|[]#");  
     espera(); 
     }
  if (digitalRead(remote)==LOW) chargeGame = true; espera();
}     
 //eof_level_600
 
if (level == 700) {  // dancing and going up
   // video1 = 8;  
   // wait for dance  
}
 //eof_level_700


if (level == 800) {   // the end
   // Serial.println("#S|MAIN_10|[]#");  
   // video1 = 9;  

   // low to the ground
   // lights
   // open the door
}
 //eof_level_800 --- END

 
if (level == 2100) {  // charge sensors TEST 
     for (int l=0; l < 6; l++) {
      if (digitalRead(charg[l])==LOW) {
        Serial.print ("Charge Sensor =");
        Serial.println(l+1);
        digitalWrite(irled[l], HIGH);
        delay(3000);
        digitalWrite(irled[l], LOW);
       }
     }
}   
// eof_level_2100


if (level == 2000) {  // PNEUMATICS TEST LEVEL
   if(digitalRead(remote)== LOW) {
    digitalWrite(airSSR,LOW);
    Serial.println(" SSR +++");
    } else { digitalWrite(airSSR,HIGH); }

   if(digitalRead(leakSig)== LOW) { 
    digitalWrite(airLOW,HIGH);
    Serial.println(" LOW ---");
    } else { digitalWrite(airLOW,LOW);  }
}
// eof_level_2000

//  showNumber(number); //Test pattern
//  number--;
  //number %= 100; //Reset x after 99
 // Serial.println(number); //For debugging
 // delay(500);

}
// eof_LOOP

