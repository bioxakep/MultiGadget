void gateUp()
{
  while (digitalRead(gateTOP)) digitalWrite(gateUP, HIGH);
  digitalWrite(gateUP, LOW);
}

void gateDown()
{

}

void cloudUp()
{
  while (digitalRead(cloudTOP)) digitalWrite(cloudUP, HIGH);
  digitalWrite(cloudUP, LOW);
}

void cloudDown()
{
  int motor = 0;
  while (motor < 50)
  { // revolutions of motor
    digitalWrite(cloudDN, HIGH);
    if (!digitalRead(cloudOOO)LOW) {
      motor++;
      while (!digitalRead(cloudOOO)) {
        ;
      }
    }
  }
  digitalWrite(cloudDN, LOW);
}

void grapeUp()
{
  int motor = 0;
  while (motor < 20 || digitalRead(grapeTOP)) {   // revolutions of motor OR top sensor
    digitalWrite(grapeUP, HIGH);
    if (!digitalRead(grapeOOO)) {
      motor++;
      while (!digitalRead(grapeOOO)) {
        ;
      }
    }
  }
  digitalWrite(grapeUP, LOW);
}

void grapeDown()
{
  int motor = 0;
  while (motor < 20 || digitalRead(grapeBOT))
  { // revolutions of motor
    digitalWrite(grapeDN, HIGH);
    if (!digitalRead(grapeOOO))
    {
      motor++;
      while (!digitalRead(grapeOOO)) {
        ;
      }
    }
  }
  digitalWrite(grapeDN, LOW);
}

void columnDown()
{
  int motor = 0;
  while (motor < 120 || digitalRead(columnBOT)) {   // revolutions of motor
    digitalWrite(columnDN, HIGH);
    if (!digitalRead(columnOOO)) {
      motor++;
      while (!digitalRead(columnOOO)) {
        ;
      }
    }
  }
  digitalWrite(columnDN, LOW);
}

void columnUp()
{
  int motor = 0;
  while (motor < 120 || digitalRead(columnTOP)) {   // revolutions of motor OR top sensor
    digitalWrite(columnUP, HIGH);
    if (!digitalRead(columnOOO)) {
      motor++;
      while (!digitalRead(columnOOO)) {
        ;
      }
    }
  }
  digitalWrite(columnUP, LOW);
}

void poseiVaultOpen()
{
  digitalWrite(poseiValv, HIGH);    //drain the water
  delay(5000);
  digitalWrite(poseiLock, LOW);     // water drained, open the top cover
  digitalWrite(poseiValv, LOW);
}

