void drawRed() {
     for(i = 0; i < NUM_LEDS; i++) 
     { leds[i]  = CRGB::Red;     }
     FastLED.show();
}

void drawBlack() {
     for(i = NUM_LEDS; i > 0 ; i--) 
     { 
      leds[i]  = CRGB::Black;  
      delay(10);
      FastLED.show();
     }
}

void drawBlue() {
     for(i = 0; i < NUM_LEDS; i++) 
     { leds[i]  = CRGB::Blue;   }
     FastLED.show();
}

void lightBlue() {
     digitalWrite(red24v, HIGH);
     digitalWrite(blu24v, LOW);
}

void lightRed() {
     digitalWrite(red24v,LOW);
     digitalWrite(blu24v,HIGH);
}

