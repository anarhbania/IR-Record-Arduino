/*     
  This is simple program: Record time (us) between change logic (LOW or HIGH).
  We don't need know IR protocol, this is simple and universal program.
  What we need?
  a) 1x ArduinoNano
  b) 1x IR receiver
  c) ArduinoIDE (tested 1.8.19)
  Schematic:
  ArduinoNano      IR
          (2) ---- (Y)
          VCC ---- (R)
          GND ---- (G)
*/

int IR = 2; // (2) INPUT IR

const int logArraySize = 300; // Max log size, if array is full, it will report the pattern to that point
const long noChangeTime = 80000; // No change in IR pin for this duration (microseconds) will mark the end of pattern

boolean logAvailable = true;  //Flag to indicate if able to log times to array
int logCount = 0; // Number of times logged to array
long lastMicros = 0; // Previous array time entry
long logArrayTime[logArraySize]; // Array of times of when the IR pin changes state

void setup()
{
  pinMode(IR, INPUT); // (2) INPUT IR

  attachInterrupt(digitalPinToInterrupt(IR), interupt, CHANGE); // Attach an interupt to log when the IR pin changes state

  Serial.begin(9600); // Run Serial
}

void loop()
{
  if(logCount > 0) // if numbers of times is not empty
  {
    if(!logAvailable || ((micros() - lastMicros) > noChangeTime)) // if logAvailable is false (have times in array) OR noChangeTime (timeout)
    {
      logAvailable = false; // make sure no more data added to array while we report it
      logCount--; // remove last number of logCount, becouse this is timeout
      
      for(int i = 0; i < logCount; i++) // for 0 to number of logCount
      {
        logArrayTime[i] = logArrayTime[i + 1] - logArrayTime[i]; // subtract times to get interval between change INPUT
        Serial.print(String(i + 1) + ") " + String(logArrayTime[i]) + " us     "); // send to Serial logArrayTime
        
        if(i % 2 == 1) Serial.println(""); // if can't completely i / 2, and have residue = 1, then send to Serial command new line
      }

      Serial.println(); // send to Serial command new line
      Serial.println(); // send to Serial command new line
      Serial.print("int timeIR[" + (String)logCount + "] = {"); // send to Serial build array
      
      for(int i = 0; i < logCount; i++) // for 0 to number of logCount
      {
        Serial.print(String(logArrayTime[i])); // send to Serial logArrayTime
        
        if(i != (logCount - 1)) Serial.print(", "); // send to Serial ,
      }
      Serial.println("};"); // send to Serial }; (end array)

      logAvailable = true; logCount = 0; // Reset array
    }
  }

  delay(10); // time for check logAvailable
}

void interupt() // Interupt to log each time the IR signal changes state
{
  if (logAvailable) // if logAvailable is true (array is empty)
  {
    long m = micros(); // get time now

    if(logCount > 0 && ((m - lastMicros) > noChangeTime)) // if array is not empty AND noChangeTime (timeout)
    {
      logAvailable = false; // no more data added to array
    }
    else
    {
      logArrayTime[logCount] = m; // save time to array
      logCount++; // set next logCount
      lastMicros = m; // save last time

      if(logCount >= logArraySize) // if logCount is bigger then logArraySize
      {
        logAvailable = false; // no more data added to array
      }
    }
  }
}
