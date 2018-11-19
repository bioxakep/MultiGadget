class StopWatchTimer {
  long startTime = 0, stopTime = 0, totalSecs = 0;
  boolean running = false;
    long setStartTime(int hours, int minutes, int seconds)
    {
      totalSecs = 1000*(hours*60*60 + minutes*60 + seconds);
      return totalSecs;
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
             elapsed = totalSecs - (mil - startTime);
             //print(str(millis() - startTime));
        }
        else {
            elapsed = totalSecs - (stopTime - startTime);
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
}
