#include <Fsm.h>

State::State(std::function<void(void)> onEnter, std::function<void(void)> onState, std::function<void(void)> onExit)
: onEnter(onEnter),
  onState(onState),
  onExit(onExit) {}

Fsm::Fsm(State* initialState)
: initialized(false), 
  currentState(initialState) {}

void Fsm::addTransition(State* stateFrom, State* stateTo, u8 event, std::function<void(void)> onTransition) {

  if (stateFrom == NULL || stateTo == NULL) return;

  transitions.push_back({
    stateFrom,
    stateTo,
    event,
    onTransition
  });
}

void Fsm::addTimedTransition(State* stateFrom, State* stateTo, u64 interval, std::function<void(void)> onTransition) {
  
  if (stateFrom == NULL || stateTo == NULL) return;

  timedTransitions.push_back({
    {
      stateFrom,
      stateTo,
      0,
      onTransition
    },
    0,
    interval
  });
}

void Fsm::trigger(const int event) {
  if (!initialized) return;

  for (auto &t : transitions) {
    if (t.stateFrom == currentState && t.event == event) {
      Fsm::makeTransition(&t);
      return;
    }
  }


  // for (u32 i = 0; i < transitions.size(); ++i)
  //   if (transitions[i].stateFrom == currentState && transitions[i].event == event) {
  //     Fsm::makeTransition(&(transitions[i]));
  //     return;
  //   }
}

void Fsm::checkTimedTransitions() {

  for (u32 i = 0; i < timedTransitions.size(); ++i) {

    TimedTransition* transition = &timedTransitions[i];

    if (transition->transition.stateFrom != currentState) continue;

    if (transition->start == 0) transition->start = millis();
    else {
      const u64 now = millis();
      if (now - transition->start >= transition->interval) {
        Fsm::makeTransition(&(transition->transition));
        transition->start = 0;
      }
    }
  }
}

void Fsm::makeTransition(Transition* transition) {

  if (transition->stateFrom->onExit != NULL)
    transition->stateFrom->onExit();

  if (transition->onTransition != NULL)
    transition->onTransition();

  if (transition->stateTo->onEnter != NULL)
    transition->stateTo->onEnter();
  
  currentState = transition->stateTo;

  unsigned long now = millis();
  for (u32 i = 0; i < timedTransitions.size(); ++i) {
    TimedTransition* ttransition = &timedTransitions[i];
    if (ttransition->transition.stateFrom == currentState)
      ttransition->start = now;
  }
}

void Fsm::loop() {

  if (!initialized) {
    initialized = true;
    if (currentState->onEnter != NULL)
      currentState->onEnter();
  }
  
  if (currentState->onState != NULL)
    currentState->onState();
    
  Fsm::checkTimedTransitions();
}