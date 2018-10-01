void checkInputs() {
Serial.print(" balloIN = "); Serial.println (digitalRead(balloIN) ? "HIGH" : "LOW"); 
Serial.print(" pressIN = "); Serial.print   (digitalRead(pressIN) ? "HIGH" : "LOW"); 
Serial.print(" demetIN = "); Serial.println (digitalRead(demetIN) ? "HIGH" : "LOW"); 
Serial.print(" vinemIN = "); Serial.print  (digitalRead(vinemIN) ? "HIGH" : "LOW"); 
Serial.print(" dioniIN = "); Serial.println(digitalRead(dioniIN) ? "HIGH" : "LOW"); 
Serial.print(" hercuIN = "); Serial.print  (digitalRead(hercuIN) ? "HIGH" : "LOW"); 
Serial.print(" thundIN = "); Serial.println(digitalRead(thundIN) ? "HIGH" : "LOW"); 
Serial.print(" poseiIN = "); Serial.print  (digitalRead(poseiIN) ? "HIGH" : "LOW"); 
Serial.print(" narciIN = "); Serial.println(digitalRead(narciIN) ? "HIGH" : "LOW"); 
Serial.print("  noteIN = "); Serial.print  (digitalRead( noteIN) ? "HIGH" : "LOW"); 
Serial.print(" octopIN = "); Serial.println(digitalRead(octopIN) ? "HIGH" : "LOW"); 
Serial.print(" afinaIN = "); Serial.print  (digitalRead(afinaIN) ? "HIGH" : "LOW"); 
Serial.print(" musesIN = "); Serial.println(digitalRead(musesIN) ? "HIGH" : "LOW"); 
Serial.print(" arphaIN = "); Serial.print  (digitalRead(arphaIN) ? "HIGH" : "LOW"); 
Serial.print(" gorgoIN = "); Serial.println(digitalRead(gorgoIN) ? "HIGH" : "LOW"); 
Serial.print(" minotIN = "); Serial.print  (digitalRead(minotIN) ? "HIGH" : "LOW"); 
Serial.print(" zodiaIN = "); Serial.println(digitalRead(zodiaIN) ? "HIGH" : "LOW"); 
Serial.print(" flowrIN = "); Serial.print  (digitalRead(flowrIN) ? "HIGH" : "LOW"); 
Serial.print(" gheraIN = "); Serial.println(digitalRead(gheraIN) ? "HIGH" : "LOW"); 
Serial.print(" triPin  = "); Serial.println  (digitalRead(triPin)  ? "HIGH" : "LOW"); 
//Serial.print(" spareIN = "); Serial.print  (digitalRead(spareIN) ? "HIGH" : "LOW"); 
}

boolean debounce(boolean prevstate ,int pin)
{
  boolean currstate = digitalRead(pin);
  if (currstate != prevstate)
  {
    delay(10);
    currstate = digitalRead(pin);
  }
  return currstate;
}


