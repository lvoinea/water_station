
typedef enum State{
  DUMMY,
  INITIALIZING,
  CALIBRATING,
  SELECTING,
  ACQUIRING,
  DELIVERING,
  FINISHED,
  SETTINGUP,
  SLEEPING,
  NOK
};
State state = SLEEPING;

int pinValue = 0;
int pinZeroPosValue = 1; 
int menu = 0;
int user_input = 0;

int nr_timers = 0;

RtcDateTime completionTime = Rtc.GetDateTime();
bool is_time_to_wake(RtcDateTime current){
  return (completionTime.Minute() < current.Minute()-1);
}

int read_user_input(){
     // Wait for data to be available on the serial bus
     while (Serial.available() <= 0) {}
     // Convert the read data to an integer value
     int user_input = Serial.parseInt();
     Serial.println(user_input);
     // Consume the carriage return
     Serial.read();
     return user_input;
}

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

  //TODO
  //test with rtc added to the circuit

  //------------------------------------------------------------ DUMMY
  if (state == DUMMY){
    for (int i = 0; i < 3; i++) { 
      pomp_on();
      delay(1000); 
      pomp_off();
      delay(500); 
    }
    state = SLEEPING;
  }
  //------------------------------------------------------------ SETTINGUP
  if (state == SETTINGUP){

    /**
     * In this state the microcontroller interacts with an user
     * over the serial interface in order to set-up the various
     * system processing parameters: 
     * 
     * - number of timers
     * - per timer time stamp
     * - number of cylinders in use (can specify less cylinders
     *   than physically available)
     * - per cylinder distance from the previous stop and number
     *   of seconds to pump.
     */


     //-------------------- Display menu
     if (menu == 0) {
       Serial.println();
       Serial.println("Top Menu:");
       Serial.println("------------");
       Serial.println(" 1 - Print current configuration");
       Serial.println(" 2 - Set cylinders");
       Serial.println(" 3 - Set water stop time");
       Serial.println(" 4 - Set timers");
       Serial.println(" 5 - Exit");
       Serial.println("-------------");
       Serial.println("Please input selection");
     } else if (menu == 2) {
       Serial.println();
       Serial.println("Cylinder Menu:");
       Serial.println("------------");
       Serial.println(" 1 - Set number of cylinders");
       Serial.println(" 2 - Set cylinder");
       Serial.println(" 3 - Back");
       Serial.println("-------------");
       Serial.println("Please input selection");
     } else if (menu == 4) {
       Serial.println();
       Serial.println("Timer Menu:");
       Serial.println("------------");
       Serial.println(" 1 - Set number of timers");
       Serial.println(" 2 - Set timer");
       Serial.println(" 3 - Back");
       Serial.println("-------------");
       Serial.println("Please input selection");
     }

     //-------------------- Acquire user input
     
     user_input = read_user_input();

     //-------------------- Handle user input
     if (menu == 0) {
        if (user_input == 1) {
         // TODO: Print current settings
         goal_manager.display();

         Serial.println();
         Serial.print("  Water stop time: ");
         Serial.println(water_stop_time);
         
         Serial.print("  Number of timers: ");
         Serial.println(nr_timers);
         Serial.println();
        }
        else if (user_input == 2) {
          menu = 2;
        }
        else if (user_input == 3) {
          Serial.println("Input the delay time for water stop.");
          water_stop_time = read_user_input();
        }
        else if (user_input == 4) {
          menu = 4;
        }
        else if (user_input == 5) {
          Serial.println("Bye, bye!"); 
          state = SLEEPING;
        }
     }
     //------ Cylinders
     else if (menu == 2) {
       if (user_input == 1) {
        Serial.println("Input the number of cylinders.");
        int nr_cylinders = read_user_input();
        goal_manager.reserve(nr_cylinders);
       }
       else if (user_input == 2) {
        Serial.println("Input the cylinder number.");
        int cylinder_number = read_user_input();
        Serial.println("Input the cylinder pomp time.");
        int running_time = read_user_input();
        goal_manager.get_goal(cylinder_number).pomp_running_time = running_time;
       }
       else if (user_input == 3) {
        menu = 0;
       }
     } 
     //------ Timers
     else if (menu == 4) {
       if (user_input == 1) {
        Serial.println("Input the number of timers.");
        nr_timers = read_user_input();
        // TODO: reserve space for timers
       }
       else if (user_input == 2) {
        Serial.println("Input the timer number.");
        int timer_number = read_user_input();
        Serial.println("Input the timer hour.");
        int timer_hour = read_user_input();
        Serial.println("Input the timer minute.");
        int timer_minute = read_user_input();
        // TODO: Update the timer
       }
       else if (user_input == 3) {
        menu = 0;
       }
     }
  }
  //------------------------------------------------------------ INITIALIZING
  if (state == INITIALIZING) {

    /**
     *  In this state the microcontroller emits a beep sequence 
     * to signal the iminent start of the pomping procedure.
     * The goal is to allow clearing the action area (i.e., take
     * your hands off).
     * Consequently, the coils of the arm motor are energized
     * and the motor is instructed to start the CALIBRATING 
     * sequence. 
     */

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
        
    // Arm motor
    cArmMotor.enableOutputs();
    cArmMotor.setMaxSpeed(1000.0);
    cArmMotor.setAcceleration(100.0);
    cArmMotor.move(ZERO_POSITION);
    cArmMotor.setSpeed(-1000);

    state = CALIBRATING;
  } else
  //------------------------------------------------------------ CALIBRATING
  if (state == CALIBRATING) {
    
    /**
     * In this state the microcontroller moves the arm clockwise and tries
     * to detect when the arm reaches the start position. When that 
     * position is reached, the switch connected to the pinZeroPos
     * will be pressed by the arm and the pin will read HIGH. At that
     * point the microcontroller will enter the goal selection state
     * (i.e., SELECTING).
     */
    
    pinZeroPosValue = digitalRead(pinZeroPos);
    if (pinZeroPosValue == HIGH){
       state = SELECTING;
    } else {
      if (cArmMotor.distanceToGo() == 0){
         // Under normal circumstances this should not happen.
         // Probably a hardware failure occured (e.g., broken arm).
         // Shutdown and go to NOK state.
         Serial.println("ERROR: Failed to calibrate.");
         shutdown(false);
      } else {
         cArmMotor.runSpeed();
      }
    }
  } else
  //------------------------------------------------------------ SELECTING
  if (state == SELECTING) {

    /**
     *  In this state the microcontroller retrieves the information required
     * to handle the next cyclinder (i.e., distance to cyclinder, time to
     * pump). If there is no cylinder left to handle, it shuts down the 
     * setup (i.e., motors, running leds) and sends the microcontroller to
     * the SLEEPING state. 
     */ 
    
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

    /**
     *  In this state the microcontroller moves the arm anti-clockwise
     * for a number of degrees in order to reach a position on 
     * top of the next cylinder. The number of degrees is cylinder
     * specific in order to enable different configurations of
     * cylynders around the central trunk.
     */
    
    //Check if the stepper reached the target position
    if (cArmMotor.distanceToGo() == 0) {
      state = DELIVERING;
    } else {
      cArmMotor.runSpeed();
    } 
    
  }  else 
  //------------------------------------------------------------ DELIVERING
  if (state == DELIVERING){

    /**
     * In this state the microcontroller triggers the pump
     * and waits for a cylinder specific period in order to fill
     * the cylinder. Consequently, the microcontroller swithces off
     * the pump and waits for a configurable amount of time in
     * order to allow the water dripping to stop before moving on
     * to the next cylinder.
     */

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
    delay(water_stop_time);
    
    state = SELECTING;
      
  } else 
  //------------------------------------------------------------ FINISHED
  if (state == FINISHED){

    /**
     * In this state the the microcontroller registers the completion
     * time of the watering procedure and then moves to the SLEEPING
     * state.
     */

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

    /**
     *  This is the state where the microcontroller will spend most of
     * its time, waiting for an event to trigger it to do some useful
     * work. These triggers can be one of the folowing:
     * 
     * - a button press signalling the need for a demo round
     * 
     * - a button press signaling the need to configure the 
     * processing parameters (i.e., the timers and the duration
     * of pumping for each cylinder).
     * 
     * - a timer match signlling the need to run the pumping
     * routine as configured.
     */

    // This will make sure the serial bus is left in a clean state
    // and avoid getting gibberish output after the wake-up.
    Serial.flush();

    /**
     *   Practically sleep for CHECK_TIME_INTERVAL * 8s.
     * The microcontroller will only check for events after this
     * interval elapsed. However, it will indicate every 8 seconds
     * that it is asleeep. 
     * 
     * NOTE: 8 seconds is the maximum duration that can be programmed
     * for the internal watchdog.
     */

    for (int i = 0; i < CHECK_TIME_INTERVAL; i++) {
      
      // Sleep for 8 seconds
      registerWatchdog (0b100001); 

      // Signal the station is alive but sleeping
      digitalWrite(pinOn, HIGH); 
      delay(BLINK_ON); 
      digitalWrite(pinOn, LOW);  
    }

    // Check for request for demo mode
    pinValue = digitalRead(pinDemo);
    if(pinValue == HIGH){
      Serial.println("Demo mode activated.");
      state = INITIALIZING;
    } 
    
    // Check for request for set-up
    pinValue = digitalRead(pinSettings);
    if(pinValue == HIGH){
      Serial.println("Update settings.");
      state = SETTINGUP;
    } 

    // Check for timers
    if (state == SLEEPING) {
      RtcDateTime now = Rtc.GetDateTime();
      if (!now.IsValid()) {
          Serial.println("RTC lost confidence in the DateTime!");
          //shutdown(false);
      } else {
         if (is_time_to_wake(now)) {
            state = INITIALIZING;
         }
      }
    }

  } else 
  //------------------------------------------------------------ NOK
  if (state == NOK){
      /**
       *  If the microcontroller reaches this state there is no way
       * out. It will stay here until it is taken down by removing
       * power. In the meantime, it will signal every 8 seconds the
       * error state by blinking the error pin.
       */

      // Blink the error led
      digitalWrite(pinError, HIGH); 
      delay(BLINK_ON); 
      digitalWrite(pinError, LOW);   

      // sleep for 8 seconds
      registerWatchdog (0b100001); 
  }
}
