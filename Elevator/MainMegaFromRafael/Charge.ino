void charging() {
// se muestra el primer irled ( 3,5,2,4,1,6 )
// se espera que empieza a cargar el primero
// si se carga todo se activa el led #2
// y se espera que se empieze a cargar el siguente bloque
// si se intenta cargar un bloque diferente se pierde toda la carga y volvemos al principio.
// se gana si se carga todos lo bloques hasta el final
// ----------- var for charging game --------------
// int bar          = 0;
// int l            = 0;
// int actual [6]   = {  3,  5,  2,  4,   1,   6 };
// int chBlock[6]   = { 26, 51, 66, 92, 132, 160 };
// int chCharged[6] = {  0,  0,  0,  0,   0,   0 };
// int actBlock     = 0;

// int irled[6]  = { 26,  28,  30,  24,  32,  34 };  
// int charg[6]  = { 48,  46,  44,  42,  40,  38 };  // hall sensors

if ( chargeGame == false || digitalRead(remote) == HIGH ) 
{ 
   if ( l <= 6) {
     digitalWrite(irled[l], HIGH); //se enciende el ir_led correspondiente
      for (int t=0; t < 6; t++) {
          if ( digitalRead(charg[l]) == LOW && digitalRead(charg[t]) == LOW ) //&& actual[l] == l-1 )   // si el hall activado es el mismo que el orden actual
          {  
            leds[bar]  = CRGB::Red;
            bar ++;      // cada led encendido
            if ( bar == chBlock[l] ) {
              digitalWrite(irled[l], LOW); //se apaga el ir_led 
              l++;
            }
            leds[bar]  = CRGB::Red;
/*            
            Serial.print (" l=");
            Serial.print (l);
            Serial.print (" t=");
            Serial.print (t);
            Serial.print (" chBlock[l]=");
            Serial.print (chBlock[l]);
            Serial.print (" bar=");
            Serial.println(bar);
*/
            FastLED.show();
            delay(30);
                    
             if ( chCharged[l-1] == chBlock[l-1] )   // if whole block is on go to the next block
               {
                 l++;
               }
             
             delay(100);
          } else if (  digitalRead(charg[l]) == HIGH && digitalRead(charg[t]) == LOW && t > l && l > 0 ) // si el bloque previo no ha sido cargado
                 {
                  Serial.println(" Delete...");
                  digitalWrite(irled[l], LOW); //se apaga el ir_led 
                  for(i = bar; i > 0 ; i--) 
                  { 
                    leds[i]  = CRGB::Black;  
                    delay(2);
                    FastLED.show();
                  }   
                  bar = 0;
                  l = 0;
                  chCharged[l] = 0;
                 }
      }
 }
  // eof_if ( l <= 7)
    if (l >= 6) {  // if WON 
      chargeGame = true;
      delay(900);
    }
}
// eof_chargeGame  

}
// eof_charging
