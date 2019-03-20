void pnUP(int howMany){
  digitalWrite(airSSR,HIGH );
  digitalWrite(airLOW,HIGH );
  delay(howMany);
  digitalWrite(airSSR,HIGH );
  digitalWrite(airLOW,LOW );
}

void pnDN(int howMany){
  digitalWrite(airSSR,LOW );
  digitalWrite(airLOW,LOW );
  delay(howMany);
  digitalWrite(airSSR,HIGH );
  digitalWrite(airLOW,LOW );
}

