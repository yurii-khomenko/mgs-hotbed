#include <Fsm.h>

State::State(std::function<void(void)> onEnter, std::function<void(void)> onState, std::function<void(void)> onExit)
    : onEnter(onEnter),
      onState(onState),
      onExit(onExit) {}

Fsm::Fsm(State *initialState)
    : initialized(false),
      currentState(initialState) {}

void
Fsm::addTransition(State *stateFrom, State *stateTo, const u8 event, const std::function<void(void)> onTransition) {

  if (stateFrom == NULL || stateTo == NULL) return;

  transitions.push_back({
                            stateFrom,
                            stateTo,
                            event,
                            onTransition
                        });
}

void Fsm::addTimedTransition(State *stateFrom, State *stateTo, const u64 interval,
                             const std::function<void(void)> onTransition) {

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

void Fsm::clearTransition() {
  timedTransitions.clear();
  timedTransitions.clear();
}

void Fsm::trigger(const u8 event) {

  if (!initialized) return;

  for (auto &t : transitions)
    if (t.stateFrom == currentState && t.event == event) {
      makeTransition(&t);
      return;
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

  checkTimedTransitions();
}

// PRIVATE
//============================================================================>

void Fsm::checkTimedTransitions() {

  for (auto &t : timedTransitions) {

    if (t.transition.stateFrom != currentState) continue;

    if (t.start == 0)
      t.start = millis();
    else if (millis() - t.start >= t.interval) {
      makeTransition(&(t.transition));
      t.start = 0;
    }
  }
}

void Fsm::makeTransition(Transition *t) {

  if (t->stateFrom->onExit != NULL)
    t->stateFrom->onExit();

  if (t->onTransition != NULL)
    t->onTransition();

  if (t->stateTo->onEnter != NULL)
    t->stateTo->onEnter();

  currentState = t->stateTo;

  for (auto &tr : timedTransitions)
    if (tr.transition.stateFrom == currentState)
      tr.start = millis();
}