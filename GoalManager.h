#define NR_GOALS 6

class Goal{
  public:
    int steps;              // Number of steps for the new target position
    int pomp_running_time;  // Number of ms to wait while pomp is running

  public:
    Goal();
    Goal(int steps_, int pomp_running_time_);
};

Goal::Goal(): steps(0), pomp_running_time(0) {}
Goal::Goal(int steps_, int pomp_running_time_): steps(steps_), pomp_running_time(pomp_running_time_){}

class GoalManager{

  Goal goal_list[NR_GOALS];
  int current_goal = 0;
  int total_goals = 0;
  
  public:
    GoalManager(){}
    void reset();
    void clear();
    int size();
    void reserve(int nr_goals);
    bool load();
    bool save();
    void display();


    Goal get_next_goal();
    Goal& get_goal(int pos);
    bool has_next_goal();
};

void GoalManager::reset(){
  current_goal = 0;
}

void GoalManager::clear(){
  total_goals = 0;
  reset();
}

void GoalManager::reserve(int nr_goals){
  clear();
  if (nr_goals <= NR_GOALS){
    total_goals = nr_goals;
  }
  else {
    total_goals = NR_GOALS;
  }
  for(int i=0; i<total_goals; i++){
    goal_list[i].steps = 1000;
    goal_list[i].pomp_running_time = 0;
  }
}

bool GoalManager::load(){
  clear();
  reserve(readIntEeprom());
  for(int i=0; i<total_goals; i++){
    goal_list[i].pomp_running_time = readIntEeprom();
  }
}

bool GoalManager::save(){
  bool result = writeIntEeprom(total_goals);
  for(int i=0; i<total_goals; i++){
    result = result and writeIntEeprom(goal_list[i].pomp_running_time);
  }
  return(result);
}

void GoalManager::display(){
   Serial.println();
   Serial.print("  Cylinders: ");
   Serial.println(total_goals);
   for (int i=0; i<total_goals; i++){
      Serial.println("  ----------");
      Serial.print("  Cyl: ");
      Serial.println(i);
      Serial.print("  Running time: ");
      Serial.println(goal_list[i].pomp_running_time);
   }
}

int GoalManager::size(){
  return total_goals;
}

bool GoalManager::has_next_goal(){
  return current_goal < total_goals;
}

Goal GoalManager::get_next_goal(){
  return goal_list[current_goal++];
}

Goal& GoalManager::get_goal(int pos){
    return goal_list[pos];
}
