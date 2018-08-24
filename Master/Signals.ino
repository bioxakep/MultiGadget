// same signal (250ms) to every prop
// same short  (250ms) from every prop

void signal2ghera() {
  digitalWrite(gheraOUT, HIGH);
  delay(250);
  digitalWrite(gheraOUT, LOW);
}

void signal2muses() {
  digitalWrite(musesOUT, HIGH);
  delay(250);
  digitalWrite(musesOUT, LOW);
}

