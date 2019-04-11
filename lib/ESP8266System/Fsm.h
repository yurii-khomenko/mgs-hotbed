#ifndef FSM_H
#define FSM_H

#include <Arduino.h>

struct State {
  State(std::function<void(void)> onEnter, std::function<void(void)> onState, std::function<void(void)> onExit);
  std::function<void(void)> onEnter;
  std::function<void(void)> onState;
  std::function<void(void)> onExit;
};

class Fsm {
  public:
    Fsm(State* initialState);
    
    void addTransition(State* stateFrom, State* stateTo, u8 event, std::function<void(void)> onTransition);
    void addTimedTransition(State* stateFrom, State* stateTo, u64 interval, std::function<void(void)> onTransition);
    void clearTransition();

    void trigger(u8 event);
    void loop();
  
  private:
    struct Transition {
      State* stateFrom;
      State* stateTo;
      u8 event;
      std::function<void(void)> onTransition;
    };
    
    struct TimedTransition {
      Transition transition;
      u64 start;
      u64 interval;
    };

    bool initialized;
    State* currentState;
    std::vector<Transition> transitions;
    std::vector<TimedTransition> timedTransitions;
    
    void checkTimedTransitions();
    void makeTransition(Transition *transition);
};

#endif