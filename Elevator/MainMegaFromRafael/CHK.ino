void checkInputs() {

Serial.println(">");

  for (int k=0; k < 6; k++) {
    Serial.print(digitalRead(charg[k]));
    Serial.print("-");
  }
Serial.println(" ");
Serial.print("Remote   = ");
Serial.println(digitalRead(remote));  //    = A0; // WHT
Serial.print("tubes    = ");
Serial.println(digitalRead(tubes));   //     = A15; // BRW
Serial.print("Emer A   = ");
Serial.println(digitalRead(emerA));   //     = 15; // ORA
Serial.print("Emer B   = ");
Serial.println(digitalRead(emerB));   //     = 14; // GRN
Serial.print("CharSigA = ");
Serial.println(digitalRead(CharSigA));//   = 4;  // RD
Serial.print("CharSigB = ");
Serial.println(digitalRead(CharSigB)); //   =  7; // GRN
Serial.print("StopHU   = ");
Serial.println(digitalRead(StopHU));  //   = 52; // BK
Serial.print("VacSig   = ");
Serial.println(digitalRead(VacSig));  //   = 5;  // BK 
Serial.print("LeakSig  = ");
Serial.println(digitalRead(leakSig));//    = 6;  // WHT
Serial.print("seatA    = ");
Serial.println(digitalRead(seatA));  //      = A13;
Serial.print("seatB    = ");
Serial.println(digitalRead(seatB));  //      = A14;
Serial.print("roof     = ");
Serial.println(digitalRead(roof));   //       = 3;
Serial.print("Level    = ");
Serial.println(level);

  
}

