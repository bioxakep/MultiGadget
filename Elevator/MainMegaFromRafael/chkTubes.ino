void chkTubes() {
 int tubesw = 0;
 
  while (digitalRead(tubes) == LOW) {
    tubesw++;
  }
  if ( tubesw > 100 ) 
     {
       if (tubesw > 150 && tubesw <200 ){
           Serial.println(" HIGH Pulse detected " );
           //signal to UNO HIGH pulse
           return;  
       } 
       else if (tubes > 400 && tubes < 600 ){
           Serial.println(" MED  Pulse detected " );
           //signal to MED high pulse
           return;  
       } 
       else if (tubes > 700){
           Serial.println(" LOW  Pulse detected " );
           //signal to UNO LOW pulse
           tubesPass = true;
           return;  
       }
     } 
}
