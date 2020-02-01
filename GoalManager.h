#include <LinkedList.h>

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

  LinkedList<Goal> goal_list;
  int current_goal = 0;
  
  public:
    GoalManager(){}
    void reset();
    void clear();
    int size();
    void add_goal(Goal goal);
    Goal get_next_goal();
    Goal get_goal(int pos);
    bool has_next_goal();
};

void GoalManager::reset(){
  current_goal = 0;
}

void GoalManager::clear(){
  goal_list.clear();
}

int GoalManager::size(){
  return goal_list.size();
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

Goal GoalManager::get_goal(int pos){
    return goal_list.get(pos);
}
