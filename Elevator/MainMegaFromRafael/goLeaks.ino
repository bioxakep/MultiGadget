void Goleaks(){
digitalWrite(toLeak, HIGH);
delay(200);
digitalWrite(toLeak, LOW);
delay(200);
digitalWrite(LeakMas, LOW);  // rel 11 OPEN - ON
delay(500);
//leaks = true;   
}
 // eof_GoLeaks
