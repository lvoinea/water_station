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
  Serial.println("> Initializing...");

  //-------------------- Control Panel
  pinMode(pinDemo, INPUT);
  pinMode(pinSettings, INPUT);

  //-------------------- Real Time Clock (DS1302)
  if(!is_error){
    pinMode(pinSDA, INPUT_PULLUP);
    pinMode(pinSCL, INPUT_PULLUP);
    rtc.begin();
  }
  //-------------------- Pomp Driver
  if(!is_error){
    pinMode(pinPompDriver, OUTPUT);
    pomp_off();
  }
  
  //-------------------- Zero position switch
  if(!is_error){
    pinMode(pinZeroPos, INPUT_PULLUP);  
  }

  //-------------------- Motor (28BYJ-48 + ULN2003)
  

  //-------------------- LED display
  if(!is_error){
    pinMode(pinError, OUTPUT);
    pinMode(pinOn, OUTPUT);
  } 

  //-------------------- Cylinder register
  if(!is_error){
    Serial.print("> Loading "); Serial.print("cylinders ... ");
    cylinder_register.load();
    Serial.println("OK");
    cylinder_register.display();
    Serial.println();
  }

  //------------------- Timer register
  if(!is_error){
    Serial.print("> Loading "); Serial.print("timers ... ");
    timer_register.load();
    Serial.println("OK");
    timer_register.display();
  }

  //------------------- Initialization done
  Serial.print("> Initialization finished at: ");
  print_date_time(rtc.getTime());
  Serial.println();
}
