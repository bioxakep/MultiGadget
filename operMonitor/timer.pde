class StopWatchTimer {
  long startTime = 0, stopTime = 0, elapsedSecs = 0;
  long passedTime = 0;
  boolean running = false;  
  boolean pause = false;
    void setStartTime(int hours, int minutes, int seconds)
    {
      elapsedSecs = 1000*(hours*60*60 + minutes*60 + seconds);
    }
    void start() 
    {
        startTime = millis();
        running = true;
    }
    void stop() 
    {
        stopTime = millis();
        running = false;
    }
    long getElapsedTime() {
        long elapsed;
        if (running) {
             long mil = millis();
             //print("millis = " + str(mil));
             //print("startTime = " + str(startTime));
             passedTime = mil - startTime;
             elapsed = elapsedSecs - (mil - startTime);
             //print(str(millis() - startTime));
        }
        else {
            elapsed = elapsedSecs - (stopTime - startTime);
        }
        return elapsed;
    }
    int second() {
      return int((getElapsedTime() / 1000) % 60);
    }
    int minute() {
      return int((getElapsedTime() / (1000*60)) % 60);
    }
    int hour() {
      return int((getElapsedTime() / (1000*60*60)) % 24);
    }
    long passedTime()
    {
      getElapsedTime();
      return passedTime;
    }
}
