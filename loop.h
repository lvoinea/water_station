
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

int menu = 0;

bool is_time_to_wake(const Time& current){
  bool time_to_wake = false;
  for(int i=0; i<timer_register.get_nr_timers(); i++){
    Timer timer = timer_register.get_timer(i);
    if ((timer.hour == current.hour) && (timer.minute == current.minute) && (current.sec < 10)){
      time_to_wake = true;
      break;
    }
  }
  return time_to_wake;
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

  //Reset the cylinder register
  cylinder_register.reset();

  // Report stop time
  Serial.print("Shutdown at: ");
  print_date_time(rtc.getTime());
  Serial.println();

  // Switch off the RUNNING sign
  digitalWrite(pinOn, LOW); 

  if (success) {
    state = FINISHED;
    playMelody(ok_melody, ok_durations, ok_nr_notes);
  } else {
    state = NOK;
    playMelody(fail_melody, fail_durations, fail_nr_notes);
  }
}

void loop() {


  //------------------------------------------------------------ DUMMY
  if (state == DUMMY){
    /**
     * This is a debug state used only during develoment.
     * It stubs the following states
     *  INITIALIZING
     *  CALIBRATING
     *  SELECTING
     *  ACQUIRING
     *  DELIVERING
     *  FINISHED
     */
     
    // TODO: Debug code fits between the lines below
    //---------------------------
    
    //---------------------------
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
    digitalWrite(pinOn, HIGH); 
    digitalWrite(pinError, HIGH); 


    //-------------------- Display menu
    if (menu == 0) {
      Serial.println();
      Serial.println("Top Menu:");
      Serial.println("------------");
      Serial.print(" 1 - "); Serial.println("Print configuration");
      Serial.print(" 2 - "); Serial.print("Configure "); Serial.println("cylinders");
      Serial.print(" 3 - "); Serial.print("Configure "); Serial.println("timers");
      Serial.print(" 4 - "); Serial.println("Set current time");
      Serial.print(" 5 - "); Serial.println("Save & exit");
      Serial.println("-------------");
      Serial.println("Please input selection");
    } 
    else if (menu == 2) {
      Serial.println();
      Serial.println("Cylinder Menu:");
      Serial.println("------------");
      Serial.print(" 1 - "); Serial.print("Set number of "); Serial.println("cylinders");
      Serial.print(" 2 - "); Serial.println("Set water stop time");
      Serial.print(" 3 - "); Serial.print("Configure "); Serial.println("cylinder");
      Serial.print(" 4 - "); Serial.println("Back");
      Serial.println("-------------");
      Serial.println("Please input selection");
    } 
    else if (menu == 3) {
      Serial.println();
      Serial.println("Timer Menu:");
      Serial.println("------------");
      Serial.print(" 1 - "); Serial.print("Set number of "); Serial.println("timers");
      Serial.print(" 2 - "); Serial.print("Configure "); Serial.println("timer");
      Serial.print(" 3 - "); Serial.println("Back");
      Serial.println("-------------");
      Serial.println("Please input selection");
    }

    //-------------------- Acquire user input
    int user_input = read_user_input();

    //-------------------- Handle user input
    if (menu == 0) {
      if (user_input == 1) {
        Serial.print("========");Serial.print("========");Serial.print("========");Serial.println();
        Serial.print("> Current time: ");
        print_date_time(rtc.getTime());
        Serial.println();

        Serial.print("> Temperature: ");
        Serial.println(rtc.getTemp());
        
        cylinder_register.display();
        timer_register.display();
        Serial.print("========");Serial.print("========");Serial.print("========");Serial.println();
      }
      else if (user_input == 2) {
        menu = 2;
      }
      else if (user_input == 3) {
        menu = 3;
      }
      else if (user_input == 4) {
        Serial.print("Input the "); Serial.print("current time (DD/MM/YY w HH:MM:SS.) "); 
        byte day, month, year, dow, hour, minute, second;
        read_date_time(day, month, year, dow, hour, minute, second);
        
        rtc.setDOW(dow);
        rtc.setTime(hour, minute, second);
        rtc.setDate(day, month, 2000 + year);
      }
      else if (user_input == 5) {
        Serial.print("Saving configuration...");
        bool saved = false;
        saved = cylinder_register.save() || saved;
        saved = timer_register.save() || saved;
        if (saved) {
          Serial.println("YES"); 
        }
        else {
          Serial.println("NO"); 
        }
        Serial.println("Bye, bye!"); 
        state = SLEEPING;
      }
    }
    //------ Cylinders
    else if (menu == 2) {
      if (user_input == 1) {
        Serial.print("Input the "); Serial.print("number of "); Serial.println("cylinders.");
        int nr_cylinders = read_user_input();
        cylinder_register.set_nr_cylinders(nr_cylinders);
      }
      else if (user_input == 2) {
        Serial.print("Input the "); Serial.println("delay time for water stop.");
        cylinder_register.water_stop_time = read_user_input();
      }
      else if (user_input == 3) {
        Serial.print("Input the "); Serial.print("cylinder "); Serial.println("number.");
        int cylinder_number = read_user_input();
        
        Serial.print("Input the "); Serial.print("cylinder "); Serial.println("pomp running time.");
        int running_time = read_user_input();
        cylinder_register.get_cylinder(cylinder_number).pomp_running_time = running_time;
      }
      else if (user_input == 4) {
        menu = 0;
      }
    } 
    //------ Timers
    else if (menu == 3) {
      if (user_input == 1) {
        Serial.print("Input the "); Serial.print("number of "); Serial.println("timers.");
        int nr_timers = read_user_input();
        timer_register.set_nr_timers(nr_timers);
      }
      else if (user_input == 2) {
        Serial.print("Input the "); Serial.print("timer "); Serial.println("number.");
        int timer_number = read_user_input();

        Serial.print("Input the "); Serial.print("timer "); Serial.println("hour.");
        int hour = read_user_input();
        timer_register.get_timer(timer_number).hour = (byte)hour;

        Serial.print("Input the "); Serial.print("timer "); Serial.println("minute.");
        int minute = read_user_input();
        timer_register.get_timer(timer_number).minute = (byte)minute;
      }
      else if (user_input == 3) {
        menu = 0;
      }
    }
    digitalWrite(pinOn, LOW); 
    digitalWrite(pinError, LOW); 
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
     * point the microcontroller will enter the cylinder selection state
     * (i.e., SELECTING).
     */
    
    int pinZeroPosValue = digitalRead(pinZeroPos);
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
    
     if (cylinder_register.has_next_cylinder()){
        current_cylinder = cylinder_register.get_next_cylinder();
        cArmMotor.move(current_cylinder.steps);
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
    print_date_time(rtc.getTime());
    Serial.println();
    
    //  Pomp the water
    pomp_on();
    delay(current_cylinder.pomp_running_time);
    pomp_off();

    // Wait for the water to stop
    delay(cylinder_register.water_stop_time);
    
    state = SELECTING;
      
  } else 
  //------------------------------------------------------------ FINISHED
  if (state == FINISHED){

    /**
     * In this state the the microcontroller registers the completion
     * time of the watering procedure and then moves to the SLEEPING
     * state.
     */

    state = SLEEPING;
     
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
    int pinDemoValue = digitalRead(pinDemo);
    if(pinDemoValue == HIGH){
      Serial.println("Demo mode activated.");
      state = INITIALIZING;
      //state = DUMMY;
    } 
    
    // Check for request for set-up
    int pinSettingsValue = digitalRead(pinSettings);
    if(pinSettingsValue == HIGH){
      Serial.println("Update settings.");
      state = SETTINGUP;
    } 

    // Check for timers
    if (state == SLEEPING) {
      if (is_time_to_wake(rtc.getTime())) {
        state = INITIALIZING;
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
