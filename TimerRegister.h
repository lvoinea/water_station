//--------------------------------------------------------------------- Timer
class Timer{

    public:
        byte hour;
        byte minute;

    private:
        int address;            // Address of timer saved in EEPROM

    public:
        Timer();
        Timer(byte hour_, byte minute_);
        void init();
        void load();
        bool save();
        void display();
};

Timer::Timer(): hour(0), minute(0) {};
Timer::Timer(byte hour_, byte minute_): hour(hour_), minute(minute_) {};

void Timer::init(){
    // Reserve EEPROM memory space for the timer

    // Reserve memory for hour
    address = reserve_byte_eeprom();
    // Reserve memory for minutes
    reserve_byte_eeprom();
}

void Timer::load(){
  // Load the timer values from the EEPROM
  int read_address;

  // Read the hour
  read_address = address;
  hour = EEPROM.readByte(read_address);

  // Read the minutes
  read_address += sizeof(byte);
  minute = EEPROM.readByte(read_address);
}

bool Timer::save(){
  // Save the timer values to the EEPROM
  int save_address;
  bool result; 

  // Save the timer hour
  save_address = address;
  result = EEPROM.updateByte(save_address, hour);

  // Save the timer minute
  save_address += sizeof(byte);
  result = EEPROM.updateByte(save_address, minute) || result;

  return(result);
}

void Timer::display(){
    Serial.print(hour);
    Serial.print(":");
    Serial.print(minute);
}

//--------------------------------------------------------------------- Timer register 
class TimerRegister{
    private:
        // Address of registry in EEPROM
        int address;
        Timer timer_list[NR_TIMERS];
        int used_timers = 0;

    public:
        TimerRegister();
        void init();
        void clear();
        int get_nr_timers();
        void set_nr_timers(int nr_timers);
        bool load();
        bool save();
        void display();
        Timer& get_timer(int pos);

};

TimerRegister::TimerRegister(){
  // For the timer registry the initialization happens
  // at the same time with the declaration. This registry
  // is actually a singletone, so the initialization 
  // happens only one time.
  init();
}

void TimerRegister::init(){
  // Reserve EEPROM memory space for the registry including
  // all available timers.
  
  // Reserve EEPROM memory for the number of timers
  address = reserve_int_eeprom();

  // Reserve EEPROM memory for the timers
  for(int i=0; i<NR_TIMERS; i++){
    timer_list[i].init();
  }
}

void TimerRegister::set_nr_timers(int nr_timers){
  if (nr_timers <= NR_TIMERS){
    used_timers = nr_timers;
  }
  else {
    used_timers = NR_TIMERS;
  }
}

bool TimerRegister::load(){
  // Load the registry values from the EEPROM
  int read_address;

  // Read the actual number of used timers
  read_address = address;
  set_nr_timers(EEPROM.readInt(read_address));

  // Loads the registered timers
  // Only the used number of timers is loaded. 
  for(int i=0; i<used_timers; i++){
    timer_list[i].load();
  }
}

bool TimerRegister::save(){
  // Save the registry values to the EEPROM
  int save_address;
  bool result; 

  // Save the actual number of used timers
  save_address = address;
  result = EEPROM.updateInt(save_address, used_timers);

  // Save the registered timers
  // Only the used number of timers is saved. 
  for(int i=0; i<used_timers; i++){
    result = timer_list[i].save() || result;
  }

  return(result);
}

void TimerRegister::display(){
  Serial.println();
  Serial.print("- Timers: ");
  if (used_timers > 0) {
    Serial.println(used_timers);
    Serial.println("  *************");
    for (int i=0; i<used_timers; i++){
      Serial.print("  Timer: ");
      Serial.print(i);
      Serial.print(" ");
      timer_list[i].display();
      Serial.println();
      Serial.println("  ----------");
    }
  } else {
    Serial.println("not configured");
  }
}

int TimerRegister::get_nr_timers(){
  return used_timers;
}

Timer& TimerRegister::get_timer(int pos){
    return timer_list[pos];
}
