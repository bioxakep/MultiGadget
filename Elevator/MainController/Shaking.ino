void Shaking(int hard) 
{

 
 isTime2shake = millis() - start2shake;

 if (level != 60 ) // OUT OF OCTOPUS level 
 { 
 
    if (isTime2shake > nextShake && moved == false) {
      nextShake = random(300, 4000);
 /*
              Serial.print("UP isTime2Shake = ");
              Serial.print (isTime2shake);
              Serial.print (" nextShake = ");
              Serial.print (nextShake);
              Serial.print (" Time = ");
              Serial.println (millis()/1000);
 */
      Move = random(80, 320); 
      pnUP(Move);
      start2shake = millis();  
      isTime2shake = millis() - start2shake;
      moved = true;
  
     } 
      
      if (isTime2shake > nextShake && moved  == true) {
 
      nextShake = random(3000, 12000);
/*
              Serial.print("DN isTime2Shake = ");
              Serial.print (isTime2shake);
              Serial.print (" nextShake = ");
              Serial.print (nextShake);
              Serial.print (" Time = ");
              Serial.println (millis()/1000);
*/
      Move = random(70, 310); 
      pnDN(Move);
      start2shake = millis();
      moved = false;
    }
  } 
  
 if (level == 60 ) //   OCTOPUS level 
 { 
 
    if (isTime2shake > nextShake && moved == false) {
      nextShake = random(300, 1000);
              Serial.print("UP isTime2Shake = ");
              Serial.print (isTime2shake);
              Serial.print (" nextShake = ");
              Serial.print (nextShake);
              Serial.print (" Time = ");
              Serial.println (millis()/1000);
      Move = random(180, 420); 
      pnUP(Move);
      start2shake = millis();  
      isTime2shake = millis() - start2shake;
      moved = true;
  
     } 
      
      if (isTime2shake > nextShake && moved  == true) {
 
      nextShake = random(1000, 5000);
              Serial.print("DN isTime2Shake = ");
              Serial.print (isTime2shake);
              Serial.print (" nextShake = ");
              Serial.print (nextShake);
              Serial.print (" Time = ");
              Serial.println (millis()/1000);
      Move = random(170, 410); 
      pnDN(Move);
      start2shake = millis();
      moved = false;
    }
  } 
  
}

