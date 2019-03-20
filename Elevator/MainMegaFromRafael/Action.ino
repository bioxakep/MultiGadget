void leerRemote(){
  Serial.println(" press Remote/Start/skip to continue...");
      while(digitalRead(remote) == HIGH) {
     // do nothing
    }
    delay(10);
    while(digitalRead(remote) == LOW) {
     // do nothing
    }
    delay(100);

}

void espera()
{ 
  while (digitalRead(remote)==LOW) {
    // do nothing...
  }
}

