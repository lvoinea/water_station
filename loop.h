
typedef enum State{
  INITIALIZING,
  CALIBRATING,
  SELECTING,
  ACQUIRING,
  DELIVERING,
  FINISHED,
  SLEEPING,
  NOK
};

int pinZeroPosValue = 1; 
State state = INITIALIZING;

RtcDateTime completionTime;

// Register watchdog
ISR (WDT_vect) 
{
  wdt_disable();  // disable watchdog
}

void registerWatchdog (const byte interval) 
{ 
  noInterrupts ();                  // make sure we don't get interrupted before we sleep

  // Set-up watchdog
  MCUSR = 0;                        // reset various flags
  WDTCSR |= 0b00011000;             // see docs, set WDCE, WDE
  WDTCSR =  0b01000000 | interval;  // set WDIE, and appropriate delay
  wdt_reset();
  
  byte adcsra_save = ADCSRA;
  ADCSRA = 0;                       // disable ADC
  power_all_disable ();             // turn off all modules
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  interrupts ();
  sleep_cpu ();                     // goto Sleep and waits for the interrupt
  ADCSRA = adcsra_save;             // stop power reduction
  power_all_enable ();              // turn on all modules
}

void shutdown(bool success=true){
  
  // Stop the motor
  cArmMotor.stop(); 
  cArmMotor.disableOutputs();
  
  // Stop the pomp
  pomp_off();

  //Reset the goal manager
  goal_manager.reset();

  // Report stop time
  Serial.print("Shutdown at: ");
  RtcDateTime now = Rtc.GetDateTime();
  printDateTime(now);
  Serial.println();

  // Switch off the RUNNING sign
  digitalWrite(pinOn, LOW); 

  if (success) {
    state = FINISHED;
  } else {
    state = NOK;
  }
}

void loop() {

  //------------------------------------------------------------ INITIALIZING
  if (state == INITIALIZING) {
    
    // Arm motor
    cArmMotor.enableOutputs();
    cArmMotor.setMaxSpeed(1000.0);
    cArmMotor.setAcceleration(100.0);
    cArmMotor.move(ZERO_POSITION);
    cArmMotor.setSpeed(-1000);

    // Signal start sequence
    int noteDuration = 1000 / 16;
    for (int i = 0; i < NUMBER_OF_BEEPS; i++) { 
      digitalWrite(pinOn, HIGH); 
      tone(pinSpeaker, NOTE_C7, noteDuration);
      delay(500);
      digitalWrite(pinOn, LOW);
      delay(500);
      noTone(pinSpeaker);
    }
    
    // Switch on the RUNNING sign
    digitalWrite(pinOn, HIGH); 
    tone(pinSpeaker, NOTE_D7, noteDuration * 4);
    delay(1000);
    noTone(pinSpeaker);

    state = CALIBRATING;
  } else
  //------------------------------------------------------------ CALIBRATING
  if (state == CALIBRATING) {
    // Detect when we reach the initial position
    pinZeroPosValue = digitalRead(pinZeroPos);
    if (pinZeroPosValue == 1){
       state = SELECTING;
    } else {
      if (cArmMotor.distanceToGo() == 0){
         //Should not get here
         //Probably hardware error (e.g., broken arm)
         //Shutdown and go to error
         Serial.println("ERROR: Failed to calibrate.");
         shutdown(false);
      } else {
         cArmMotor.runSpeed();
      }
    }
  } else
  //------------------------------------------------------------ SELECTING
  if (state == SELECTING) {
     if (goal_manager.has_next_goal()){
        current_goal = goal_manager.get_next_goal();
        cArmMotor.move(current_goal.steps);
        cArmMotor.setSpeed(1000);
        state = ACQUIRING;
      } else {
        shutdown();
      }
  } else 
  //------------------------------------------------------------ ACQUIRING
  if (state == ACQUIRING) {
    
    //Check if the stepper reached the target position
    if (cArmMotor.distanceToGo() == 0) {
      state = DELIVERING;
    } else {
      cArmMotor.runSpeed();
    } 
    
  }  else 
  //------------------------------------------------------------ DELIVERING
  if (state == DELIVERING){

      Serial.print("Delivering at: ");
      
      //Log time
      RtcDateTime now = Rtc.GetDateTime();
      printDateTime(now);
      Serial.println();
      
      //  Pomp the water
      pomp_on();
      delay(current_goal.pomp_running_time);
      pomp_off();

      // Wait for the water to stop
      delay(current_goal.water_stop_time);
      
      state = SELECTING;
      
  } else 
  //------------------------------------------------------------ FINISHED
  if (state == FINISHED){

      RtcDateTime now = Rtc.GetDateTime();
      if (!now.IsValid()) {
          Serial.println("RTC lost confidence in the DateTime!");
          shutdown(false);
      } else {
        completionTime = now;
        state = SLEEPING;
      }
     
  } else 
  //------------------------------------------------------------ SLEEPING
  if (state == SLEEPING){

    // Practically sleep for CHECK_TIME_INTERVAL * 8s
    for (int i = 0; i < CHECK_TIME_INTERVAL; i++) {
      
      // Sleep for 8 seconds
      registerWatchdog (0b100001); 

      // Signal the station is alive but sleeping
      digitalWrite(pinOn, HIGH); 
      delay(BLINK_ON); 
      digitalWrite(pinOn, LOW);  
    }


    // Check if it is time to wakeup
    RtcDateTime now = Rtc.GetDateTime();
    if (!now.IsValid()) {
        Serial.println("RTC lost confidence in the DateTime!");
        shutdown(false);
    } else {
       if (is_time_to_wake(completionTime, now)) {
          state = INITIALIZING;
       }
    }

  } else 
  //------------------------------------------------------------ NOK
  if (state == NOK){
      // Blink the error led
      digitalWrite(pinError, HIGH); 
      delay(BLINK_ON); 
      digitalWrite(pinError, LOW);   

      // sleep for 8 seconds
      registerWatchdog (0b100001); 
  }
}
