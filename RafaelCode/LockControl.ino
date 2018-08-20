void openLocks() {
digitalWrite(balloOUT, LOW);
digitalWrite(pressOUT, LOW);
digitalWrite(demetOUT, LOW);
digitalWrite(demetHD,  LOW);
digitalWrite(vinemOUT, LOW);
digitalWrite(dioniOUT, LOW);
digitalWrite(dioniHD1, LOW);
digitalWrite(dioniHD2, LOW);         
digitalWrite(hercuHD,  LOW);
digitalWrite(molniOUT, LOW);
digitalWrite(poseiOUT, LOW);
digitalWrite(poseiHD,  LOW);
digitalWrite(narciOUT, LOW); 
digitalWrite(noteOUT,  LOW);
digitalWrite(noteHD,   LOW);   
digitalWrite(octopOUT, LOW); 
digitalWrite(afinaOUT, LOW); 
digitalWrite(afinaHD1, LOW); 
digitalWrite(afinaHD2, LOW);
digitalWrite(musesOUT, LOW);
digitalWrite(arphaHD,  LOW);
digitalWrite(gorgoOUT, LOW);
digitalWrite(gorgoHD,  HIGH);
digitalWrite(minotOUT, LOW);      
digitalWrite(minotHD,  HIGH);
digitalWrite(zodiaHD,  HIGH);
digitalWrite(flowrOUT, LOW);
digitalWrite(gheraOUT, LOW);
delay(200);
digitalWrite(gorgoHD,  LOW);
digitalWrite(minotHD,  LOW);
digitalWrite(zodiaHD,  LOW);
}

