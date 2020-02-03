/**
 * This file configures the phisical and logical resources.
 * 
 * Configuration is done per resource.
 * 
 */

boolean is_error = false;

void setup() {
    
  //-------------------- Serial
  // Open serial communications and wait for port to open:
  Serial.begin(SERIAL_SPEED);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Initializing...");

  //-------------------- Control Panel
  pinMode(pinDemo, INPUT);
  pinMode(pinSettings, INPUT);

  //-------------------- Real Time Clock (DS1302)
  if(!is_error){
    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    printDateTime(compiled);
    Serial.println();

    if (!Rtc.IsDateTimeValid()) 
    {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }

    if (Rtc.GetIsWriteProtected())
    {
        Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now <= compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) 
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    
  }
  //-------------------- Pomp Driver
  if(!is_error){
    pinMode(pinPompDriver, OUTPUT);
    pomp_off();

    water_stop_time = 1000;
  }
  
  //-------------------- Zero position switch
  if(!is_error){
    pinMode(pinZeroPos, INPUT);  
  }

  //-------------------- Motor (28BYJ-48 + ULN2003)
  

  //-------------------- Display
  if(!is_error){
    pinMode(pinError, OUTPUT);
    pinMode(pinOn, OUTPUT);
  } 

  //-----------------------cGoal manager
  if(!is_error){
    Serial.print("Register goals ...");
    goal_manager.load();
    goal_manager.display();
    
    water_stop_time = readIntEeprom();
    Serial.println();
    Serial.print("  Water stop time: ");
    Serial.println(water_stop_time);

    Serial.println("OK");
  }

  //------------------- Timers
  if(!is_error){
    Serial.print("Register goals ...");

    //TODO: Read timers from EEPROM

    Serial.println("OK");
  }

  //------------------- Initialization done
  Serial.println("Initialization OK");
  RtcDateTime now = Rtc.GetDateTime();
  printDateTime(now);
  Serial.println();
}
