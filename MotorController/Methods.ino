void megaColumnUpCom(){
   digitalWrite(megaColumn, HIGH);
   delay(30);
   digitalWrite(megaColumn, LOW);
   delay(1500);
   digitalWrite(megaColumn, HIGH);
   delay(40);
   digitalWrite(megaColumn, LOW);
   delay(2000);
   digitalWrite(megaColumn, HIGH);
   delay(50);
   digitalWrite(megaColumn, LOW);
   delay(2500);
   digitalWrite(megaColumn, HIGH);
   delay(60);
   digitalWrite(megaColumn, LOW);
   delay(3000);
   digitalWrite(megaColumn, HIGH);

}

void curtainDownCom() {
 long startTime = millis();

  while ( startTime + 10000 > millis() ) {
   digitalWrite(curtain2DN, LOW);
     if ( startTime +5000 > millis() ) digitalWrite(curtain1DN, LOW); else digitalWrite(curtain1DN, HIGH);
     if ( startTime  +800 > millis() )  {
        if ( digitalRead(curtain1TOP)) digitalWrite(curtain1DN, HIGH);
        if (!digitalRead(curtain2TOP)) digitalWrite(curtain2DN, HIGH);
     }
   }
 digitalWrite(curtain2DN, HIGH);
 digitalWrite(curtain1DN, HIGH); 
 digitalWrite(curtain2UP, HIGH); 
 digitalWrite(curtain1UP, HIGH); 
 curtainMove = true;
 Serial.println("Both Curtains down by timer...");
  
}

void curtainUpCom() {
 if (curtainMove == true) {
  if ( digitalRead(curtain1TOP))  curtain1OnTop = true; else curtain1OnTop = false;   
  if (!digitalRead(curtain2TOP))  curtain2OnTop = true; else curtain2OnTop = false;
  while (!digitalRead(curtain1TOP) || digitalRead(curtain2TOP) ) {
    if (digitalRead(curtain2TOP))    {  
      digitalWrite(curtain2UP, LOW); 
      } else {  
        digitalWrite(curtain2UP, HIGH); 
        curtain2OnTop = true;
        //Serial.println("Curtain 2 on TOP");
      } 
    if (!digitalRead(curtain1TOP))   {  
      digitalWrite(curtain1UP, LOW); 
      } else {  
        digitalWrite(curtain1UP, HIGH); 
        curtain1OnTop = true;
        //Serial.println("Curtain 1 on TOP");
      } 
   }
 Serial.println("Both Curtains on TOP");
 digitalWrite(curtain2UP, HIGH); 
 digitalWrite(curtain1UP, HIGH); 
 digitalWrite(curtain2DN, HIGH);
 digitalWrite(curtain1DN, HIGH); 
 }  
}

void cloudUpCom() {         //  send the cloud to the sky
 if ( cloudOnTop == false ) {  
    cloudGoesUp = true;
    Serial.println("cloud is movin to Top ...");
    digitalWrite(cloudUP, LOW);
  }
}


void cloudDownCom() {       // start move cloud to the bottom if its on top
  if (cloudOnTop == true ) {
    digitalWrite(cloudDN, LOW);
    Serial.println("cloud is movin Down ...");
    cloudTimer = millis();
  }
}

void grapeUpCom(){          // position to hold from start of the game
  while(grapeOnTop == false) {        
    Serial.println("\ngrapeUp Command started  " + String(millis()));
    digitalWrite(grapeUP, HIGH);
    while(  digitalRead(grapeTOP)==LOW ) {;}
    Serial.println("grape on TOP");
    grapeOnTop = true; 
    grapeOnBot = false; 
  } 
  digitalWrite(grapeUP, LOW);
  digitalWrite(grapeDN, LOW);
  Serial.println("grapeUp Command  done  " + String(millis()));
}

void grapeDownCom() {                  /// if step is passed
  while (grapeOnBot == false) {          
    Serial.println("\ngrapeDown Command started  " + String(millis()));
    digitalWrite(grapeDN, HIGH);
    while(  digitalRead(grapeBOT)==LOW ) {;}
    Serial.println("grape on BOT");
    grapeOnBot = true; 
    grapeOnTop = false; 
  } 
  digitalWrite(grapeDN, LOW);
  digitalWrite(grapeUP, LOW);
  Serial.println("grapeDown Command done " + String(millis()));
}


void columnUpCom() {                    /// if step is passed, after the eartquake
 int mover = 0;
 while (columnOnTop == false) {    
    Serial.println("\ncolumnUpCommand started... " + String(millis()));
    digitalWrite(columnUP, HIGH);
      while(  digitalRead(columnTOP)==LOW && mover < 1300) {
        mover++;
          if (mover > 500 && mover < 510 ) {  
            digitalWrite(columnUP, LOW);
            delay(1000); 
            digitalWrite(columnUP, HIGH);
            mover = 550; 
          }
        Serial.println(mover);
      }
   
    Serial.println("Column on TOP (broken) ready to be refilled or won");
    columnOnTop = true; 
    columnOnBot = false;   
    // command = 0x0x;
  digitalWrite(columnUP, LOW);
  Serial.println("columnUpCommand done. "  + String(millis()));
 }
}

void columnDownCom() {           // position to play since start of the game
 while (columnOnBot == false) {           
    Serial.println("\nCommand columnDown started... " + String(millis()));
    digitalWrite(columnDN, HIGH);
    while(  digitalRead(columnBOT)==LOW ) { ; }
    columnOnBot = true; 
    columnOnTop = false;  
    //command = 0x0x;
  } 
  digitalWrite(columnDN, LOW);
  Serial.println("Command columnDown done.  ready to play " + String(millis()));
}


void poseiVaultOpen() {
// digitalWrite(poseiValv, LOW);    //drain the water
// delay(2000);
 digitalWrite(poseiLock, HIGH);     // water drained, open the top cover
// digitalWrite(poseiValv, HIGH);
//UNLOAD GATE RELAY,OPTIONAL
 digitalWrite(gateUP, LOW);   
}

void checkInputs() {
  Serial.print("remote1   = ");
  Serial.print(digitalRead(remote1) ? "HIGH " : "LOW ");
  Serial.print("  remote2   = ");
  Serial.println(digitalRead(remote2) ? "HIGH " : "LOW ");
  Serial.print("remote3   = ");
  Serial.print(digitalRead(remote3) ? "HIGH " : "LOW ");
  Serial.print("  remote4   = ");
  Serial.println(digitalRead(remote4) ? "HIGH" : "LOW");
  Serial.println("=====================");
  Serial.print("columnTOP = ");
  Serial.println(digitalRead(columnTOP) ? "HIGH" : "LOW");
  Serial.print("columnBOT = ");
  Serial.println(digitalRead(columnBOT) ? "HIGH" : "LOW");
  Serial.println("=====================");
  Serial.print("grapeTOP  = ");
  Serial.println(digitalRead(grapeTOP) ? "HIGH" : "LOW");
  Serial.print("grapeBOT  = ");
  Serial.println(digitalRead(grapeBOT) ? "HIGH" : "LOW");
  Serial.println("=====================");
  Serial.print("cloudTOP  = ");
  Serial.println(digitalRead(cloudTOP) ? "HIGH" : "LOW");
  Serial.println("=====================");
  Serial.print("curt1TOP  = ");
  Serial.println(digitalRead(curtain1TOP) ? "HIGH" : "LOW");
  Serial.print("curt2TOP  = ");
  Serial.println(digitalRead(curtain2TOP) ? "HIGH" : "LOW");
}

void remoteControl() {

if (digitalRead(remote3) == LOW ){
megaColumnUpCom();
//grapeUpCom();
}

if (digitalRead(remote1) == LOW ){
digitalWrite(megaColumn, LOW);
// grapeDownCom();
}

if (digitalRead(remote4) == LOW ){
//   cloudUpCom();
   digitalWrite(cloudUP, LOW);
    Serial.println("\nRemote control UP");

 } else  digitalWrite(cloudUP, HIGH);
 
if (digitalRead(remote2) == LOW ){
   //cloudDownCom();
   digitalWrite(cloudDN, LOW);
    Serial.println("\nRemote control DOWN");
 } else digitalWrite(cloudDN, HIGH);

}
