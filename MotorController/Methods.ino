void curtain1UpCom() {   /// normally automatic on command   , same for 2
 Serial.println("Curtain 1 to top Command...");
 if (curtain1OnTop == false) {
     while(digitalRead(curtain1TOP) == HIGH) {
     digitalWrite(curtain1UP, LOW);
    }
   digitalWrite(curtain1UP, HIGH);
 Serial.println("Curtain 1 is on top and Done.");
 curtain1OnTop = true;
 curtain1OnBot = false;
 }
}

void curtain1DnCom() {   /// manual only , same for 2
 Serial.println("Curtain 1 to bottom Command...");
     while(digitalRead(remote1) == LOW) {
     digitalWrite(curtain1DN, LOW);
    }
   digitalWrite(curtain1DN, HIGH);
 Serial.println("Curtain 1 is on bottom.");
 curtain1OnTop = false;
 curtain1OnBot = true;
 
 }


void gateUpCom()
{
  digitalWrite(gateUP, HIGH);
}

void cloudUpCom() {         // remote send the cloud to the sky
  if (digitalRead(cloudTOP) == HIGH && digitalRead(remote2)==LOW) {  
    while(digitalRead(cloudTOP) == HIGH ) digitalWrite(cloudUP, LOW);
    digitalWrite(cloudUP, HIGH);
    Serial.println("cloud is moved to Top position");
    cloudOnTop = true;
    cloudOnBot = false;
  }
}


void cloudDownCom() {       // move cloud to the bottom if its on top only (no sensors)
  if (cloudOnTop == true ) {
    digitalWrite(cloudDN, LOW);
    delay(5000);  // timer to go down 
    digitalWrite(cloudDN, HIGH);
    cloudOnBot = true;
    cloudOnTop = false;
  }
}


void grapeUpCom(){          // position to hold from start of the game
  while(grapeOnTop == false) {        
    Serial.println("\ngrapeUp Command started");
    digitalWrite(grapeUP, HIGH);
    while(  digitalRead(grapeTOP)==LOW ) {;}
    Serial.println("grape on TOP");
    grapeOnTop = true; 
    grapeOnBot = false; 
  } 
  digitalWrite(grapeUP, LOW);
  Serial.println("grapeUp Command  done");
}

void grapeDownCom() {                  /// if step is passed
  while (grapeOnBot == false) {          
    Serial.println("\ngrapeDown Command  started");
    digitalWrite(grapeDN, HIGH);
    while(  digitalRead(grapeBOT)==LOW ) {;}
    Serial.println("grape on BOT");
    grapeOnBot = true; 
    grapeOnTop = false; 
  } 
  digitalWrite(grapeDN, LOW);
  Serial.println("grapeDown Command  done");
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
   
    Serial.println("\nColumn on TOP (broken) ready to be refilled or won");
    columnOnTop = true; 
    columnOnBot = false;   
    // command = 0x0x;
  
  digitalWrite(columnUP, LOW);
  Serial.println("\ncolumnUpCommand done. "  + String(millis()));
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
  Serial.println("Command columnDown done.  redy to play " + String(millis()));
}


void poseiVaultOpen()
{
  digitalWrite(poseiValv, HIGH);    //drain the water
  delay(5000);
  digitalWrite(poseiLock, LOW);     // water drained, open the top cover
  digitalWrite(poseiValv, LOW);
  digitalWrite(gateUP, LOW);        // UNLOAD GATE RELAY, OPTIONAL

}

void checkInputs()
{
  Serial.print("remote1   = ");
  Serial.print(digitalRead(remote1) ? "HIGH " : "LOW ");
  Serial.print("  remote2   = ");
  Serial.println(digitalRead(remote2) ? "HIGH " : "LOW ");
  Serial.print("remote3   = ");
  Serial.print(digitalRead(remote3) ? "HIGH " : "LOW ");
  Serial.print("  remote4   = ");
  Serial.println(digitalRead(remote4) ? "HIGH" : "LOW");
  Serial.print("\ngateTOP   = ");
  Serial.println(digitalRead(gateTOP) ? "HIGH" : "LOW");
  Serial.print("columnTOP = ");
  Serial.println(digitalRead(columnTOP) ? "HIGH" : "LOW");
  Serial.print("columnBOT = ");
  Serial.println(digitalRead(columnBOT) ? "HIGH" : "LOW");
  Serial.print("grapeTOP  = ");
  Serial.println(digitalRead(grapeTOP) ? "HIGH" : "LOW");
  Serial.print("grapeBOT  = ");
  Serial.println(digitalRead(grapeBOT) ? "HIGH" : "LOW");
  Serial.print("cloudTOP  = ");
  Serial.println(digitalRead(cloudTOP) ? "HIGH" : "LOW");
  Serial.print("cloudOOO  = ");
  Serial.println(digitalRead(cloudOOO) ? "HIGH" : "LOW");
  Serial.print("curt1TOP  = ");
  Serial.println(digitalRead(curtain1TOP) ? "HIGH" : "LOW");
  Serial.print("curt2TOP  = ");
  Serial.println(digitalRead(curtain2TOP) ? "HIGH" : "LOW");
}

void remoteControl() {
 
  if (digitalRead(remote1)==LOW &&  grapeOnTop == false) {            // manual  control by radio remote control
    Serial.println("\nRemote control (1)-D grapeUp");
    grapeUpCom();
  }  

  if (digitalRead(remote2)==LOW &&  grapeOnBot == false) {           // manual  control by radio remote control
    Serial.println("\nRemote control (2)-C grapeDown");
    grapeDownCom();
  }
  
  if (digitalRead(remote3)==LOW &&  columnOnTop == false) {            // manual  control by radio remote control
    Serial.println("\nRemote control (3) column up");
    columnUpCom();
  }
  
  if (digitalRead(remote4)==LOW &&  columnOnBot == false) {           // manual  control by radio remote control
    Serial.println("\nRemote control (4) column down");
     columnDownCom();
  }
}
