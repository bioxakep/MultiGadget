void arduinoConnect()
{
  connected = false;
  print("Connecting to Bridge: ");
  portName = "COM18"; // COM3 or /dev/tty.wchusbserial1410 or /dev/tty.wchusbserial1420
  arduino = new Serial(this, portName, 9600);
  long startConnect = millis();
  while (!connected && (millis() - startConnect < 60000))
  {
    arduino.write("startMonitor\n");
    //println("Connecting...");
    long now = millis();
    while (millis() - now < 1000) {
      ;
    }
    String input = getInput(false);
    if (input.length() > 4)
    {
      if (input.trim().equals("startBridge")) 
      {
        connected = true;
        println("true");
      }
    }
  }
  if (!connected) { 
    noLoop();
    println("No connection for Bridge...");
    text("NO BRIDGE CONNECTED TO " + portName, width/2, height/2);
  }
}
