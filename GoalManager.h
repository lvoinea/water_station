#include <LinkedList.h>

class Goal{
  public:
    int steps;              // Number of steps for the new target position
    int pomp_running_time;  // Number of ms to wait while pomp is running
    int water_stop_time;    // Numer of seconds to wait for the water to stop, after pomp stop

  public:
    Goal();
    Goal(int steps_, int pomp_running_time_, int water_stop_time_);
};

Goal::Goal(): steps(0), pomp_running_time(0), water_stop_time(0) {}
Goal::Goal(int steps_, int pomp_running_time_, int water_stop_time_): steps(steps_), pomp_running_time(pomp_running_time_), water_stop_time(water_stop_time_){}

class GoalManager{

  LinkedList<Goal> goal_list;
  int current_goal = 0;
  
  public:
    GoalManager(){}
    void reset();
    void clear();
    void add_goal(Goal goal);
    Goal get_next_goal();
    bool has_next_goal();
};

void GoalManager::reset(){
  current_goal = 0;
}

void GoalManager::clear(){
  goal_list.clear();
}

void GoalManager::add_goal(Goal goal){
  goal_list.add(goal);
}

bool GoalManager::has_next_goal(){
  return current_goal < goal_list.size();
}

Goal GoalManager::get_next_goal(){
  current_goal += 1;
  return goal_list.get(current_goal-1);
}
