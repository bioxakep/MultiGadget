void charging() {  // the game after CHARGER
  // gtame order ( 3,5,2,4,1,6 )

  if ( chargeGame == false )
  {

    long fromirchange = millis() - irtime;

    if ( l <= 6)
    {

      if ( fromirchange > 500 && irON[l] == true ) {
        digitalWrite(irled[l], LOW); //se enciende el ir_led correspondiente
        irtime = millis();
        irON[l] = false;
        fromirchange = 0;
      }

      if ( fromirchange > 500 && irON[l] == false ) {
        digitalWrite(irled[l], HIGH);  //se apaga el ir_led correspondiente
        irtime = millis();
        irON[l] = true;
        fromirchange = 0;
      }

      for (int t = 0; t < 6; t++)
      {
        if ( digitalRead(charg[l]) == LOW && digitalRead(charg[t]) == LOW ) //&& actual[l] == l-1 )   // si el hall activado es el mismo que el orden actual
        {
          leds[bar]  = CRGB::Red;
          bar ++;      // cada led encendido
          if (soundOK == true) {
            mp3_play(111);  // should start playing at the begining of each charging sessions, now play only at the end of each charged block
            soundOK = false;
          }
          if ( bar == chBlock[l] ) {
            digitalWrite(irled[l], LOW); //se apaga el ir_led
            irON[l] == false;
            l++;
            soundOK = true;
          }
          leds[bar]  = CRGB::Red;

          FastLED.show();
          delay(30);

          if ( chCharged[l - 1] == chBlock[l - 1] ) // if whole block is on go to the next block
          {
            l++;
          }
          
          delay(100);
        } else if (  digitalRead(charg[l]) == HIGH && digitalRead(charg[t]) == LOW && t > l && l > 0 ) // si el bloque previo no ha sido cargado
        {
          //Serial.println(" Delete...");
          digitalWrite(irled[l], LOW); //se apaga el ir_led
          mp3_play(112);
          for (i = bar; i > 0 ; i--)
          {
            leds[i]  = CRGB::Black;
            delay(2);
            FastLED.show();
          }
          bar = 0;
          l = 0;
          chCharged[l] = 0;
          delay(333);
        }
      }
    }
    // eof_if ( l <= 7)

    if (l >= 6) {  // if WON
      chargeGame = true;
      delay(100);
      //level=90;
    }

  }
  // eof_chargeGame

}
// eof_charging
