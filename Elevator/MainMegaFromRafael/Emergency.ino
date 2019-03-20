void emergency() {
  //drawBlue();
  digitalWrite(HDlight,    HIGH); // rel 6 HIGH = ON, Head Fans and lights
  delay(200);
  digitalWrite(door,       LOW);  // rel 7 door LOW = OPEN
  delay(100);
  digitalWrite(sw12v  ,    HIGH); // rel 8 sw lights
  delay(200);
  digitalWrite(fans,       HIGH);  // rel 9  fans
  delay(100);
  digitalWrite(focus,      HIGH);  // rel 10 HIGH = ON, small light
  delay(200);
  digitalWrite(LeakMas,    LOW);  // rel 11 
  delay(100);
  digitalWrite(handlockA,  LOW);   // rel 12
  delay(200);
  digitalWrite(handlockB,  LOW);  // rel 13
  delay(100);
  digitalWrite(blu24v ,    HIGH);  // rel 15  blue 24v light
  delay(200);
  digitalWrite(red24v,     LOW);  // rel 16  red 24v light
  delay(100);

  while(!digitalRead(remote)){     }
  delay(500);
  level = lastlevel;
}

