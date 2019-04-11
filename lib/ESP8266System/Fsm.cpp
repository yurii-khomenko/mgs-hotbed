#include <Fsm.h>

State::State(std::function<void(void)> onEnter, std::function<void(void)> onState, std::function<void(void)> onExit)
: onEnter(onEnter),
  onState(onState),
  onExit(onExit) {}

Fsm::Fsm(State* initialState)
: currentState(initialState),
  transitions(NULL),
  initialized(false) {}

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
      IDLE,
      onTransition
    },
    IDLE,
    interval
  });
}

void Fsm::trigger(const int event) {
  if (initialized)
    for (u32 i = 0; i < transitions.size(); ++i)
      if (transitions[i].stateFrom == currentState && transitions[i].event == event) {
        Fsm::makeTransition(&(transitions[i]));
        return;
      }
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

void Fsm::loop() {

  if (!initialized) {
    initialized = true;
    if (currentState->onEnter != NULL)
      currentState->onEnter();
  }
  
  if (currentState->onState != NULL)
    currentState->onState();
    
  Fsm::check_timed_transitions();
}

void Fsm::makeTransition(Transition* transition) {

  if (transition->state_from->on_exit != NULL)
    transition->state_from->on_exit();

  if (transition->on_transition != NULL)
    transition->on_transition();

  if (transition->state_to->on_enter != NULL)
    transition->state_to->on_enter();
  
  m_current_state = transition->state_to;

  unsigned long now = millis();
  for (u32 i = 0; i < m_timed_transitions.size(); ++i) {
    TimedTransition* ttransition = &m_timed_transitions[i];
    if (ttransition->transition.state_from == m_current_state)
      ttransition->start = now;
  }
}