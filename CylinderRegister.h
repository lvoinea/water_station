
//------------------------------------------------------------------- Cylinder
class Cylinder{
  public:
    int steps;              // Number of steps for the new target position
    int pomp_running_time;  // Number of ms to wait while pomp is running

  private:
    int address;            // Address of cylinder saved in EEPROM

  public:
    Cylinder();
    Cylinder(int steps_, int pomp_running_time_);
    void init();
    void load();
    bool save();
};

Cylinder::Cylinder(): steps(0), pomp_running_time(0), address(0) {}
Cylinder::Cylinder(int steps_, int pomp_running_time_): steps(steps_), pomp_running_time(pomp_running_time_), address(0) {}

void Cylinder::init(){
   // Reserve EEPROM memory space for the cylinder

   // Only pomp running time is saved, so reserve memory for 
   // one int only.
  address = reserve_int_eeprom();
}

void Cylinder::load(){
  // Load the cylinder values from the EEPROM
  int read_address;

  // Read the pomp running time
  read_address = address;
  pomp_running_time = EEPROM.readInt(read_address);
}

bool Cylinder::save(){
  // Save the cylinder values to the EEPROM
  int save_address;
  bool result; 

  // Save the pomp running time
  save_address = address;
  result = EEPROM.updateInt(save_address, pomp_running_time);

  return(result);
}
//--------------------------------------------------------------------- Cylinder Register
class CylinderRegister{

  public:
    // Number of miliseconds to wait for the water to stop dripping
    int water_stop_time;

  private:
    // Address of registry in EEPROM
    int address;
    Cylinder cylinder_list[NR_CYLINDERS];
    int current_cylinder = 0;
    int used_cylinders = 0;
  
  public:
    CylinderRegister();
    void init();
    void reset();
    void clear();
    int get_nr_cylinders();
    void set_nr_cylinders(int nr_cylinders);
    bool load();
    bool save();
    void display();

    Cylinder get_next_cylinder();
    Cylinder& get_cylinder(int pos);
    bool has_next_cylinder();
};

CylinderRegister::CylinderRegister(){
  // For the cylinder registry the initialization happens
  // at the same time with the declaration. This registry
  // is actually a singletone, so the initialization 
  // happens only one time.
  init();

  water_stop_time = 1000;
}

void CylinderRegister::init(){
  // Reserve EEPROM memory space for the registry including
  // all physically available cylinders.
  
  // Reserve EEPROM memory for the number of cylinders
  address = reserve_int_eeprom();
  // Reserve EEPROM memory for the water_stop_time
  reserve_int_eeprom();
  // Reserve EEPROM memory for the cylinders
  for(int i=0; i<NR_CYLINDERS; i++){
    cylinder_list[i].init();
  }
}

void CylinderRegister::reset(){
  current_cylinder = 0;
}

void CylinderRegister::set_nr_cylinders(int nr_cylinders){
  if (nr_cylinders <= NR_CYLINDERS){
    used_cylinders = nr_cylinders;
  }
  else {
    used_cylinders = NR_CYLINDERS;
  }
}

bool CylinderRegister::load(){
  // Load the registry values from the EEPROM
  reset();
  int read_address;

  // Read the actual number of used cylinders
  read_address = address;
  used_cylinders = EEPROM.readInt(read_address);

  // Read the water_stop_time
  read_address += sizeof(int);
  water_stop_time = EEPROM.readInt(read_address);

  // Loads the registered cylinders
  // Only the used number of cylinders is loaded. 
  for(int i=0; i<used_cylinders; i++){
    cylinder_list[i].load();
  }
}

bool CylinderRegister::save(){
  // Save the registry values to the EEPROM
  int save_address;
  bool result; 

  // Save the actual number of used cylinders
  save_address = address;
  result = EEPROM.updateInt(save_address, used_cylinders);

  // Save the water stop time
  save_address += sizeof(int);
  result = EEPROM.updateInt(save_address, water_stop_time) || result;

  // Save the registered cylinders
  // Only the used number of cylinders is saved. 
  for(int i=0; i<used_cylinders; i++){
    result = cylinder_list[i].save() || result;
  }

  return(result);
}

void CylinderRegister::display(){
  Serial.println();
  Serial.print("- Cylinders: ");
  if (used_cylinders > 0) {
    Serial.println(used_cylinders);
    Serial.println("  *************");
    for (int i=0; i<used_cylinders; i++){
      Serial.print("  Cylinder: ");
      Serial.println(i);
      Serial.print("  Running time: ");
      Serial.println(cylinder_list[i].pomp_running_time);
      Serial.println("  ----------");
    }
    Serial.println();
    Serial.print("- Water stop time: ");
    Serial.println(water_stop_time);
  } else {
    Serial.println("not configured");
  }
}

int CylinderRegister::get_nr_cylinders(){
  return used_cylinders;
}

bool CylinderRegister::has_next_cylinder(){
  return current_cylinder < used_cylinders;
}

Cylinder CylinderRegister::get_next_cylinder(){
  return cylinder_list[current_cylinder++];
}

Cylinder& CylinderRegister::get_cylinder(int pos){
    return cylinder_list[pos];
}
