#include <Fsm.h>
#include <Arduino.h>

State::State(std::function<void(void)> on_enter, std::function<void(void)> on_state, std::function<void(void)> on_exit)
: on_enter(on_enter),
  on_state(on_state),
  on_exit(on_exit) {}

Fsm::Fsm(State* initial_state)
: m_current_state(initial_state),
  m_transitions(NULL),
  m_num_transitions(0),
  m_num_timed_transitions(0),
  m_initialized(false) {}

Fsm::~Fsm()
{
}

void Fsm::add_transition(State* state_from, State* state_to, int event, std::function<void(void)> on_transition) {

  if (state_from == NULL || state_to == NULL) return;

  m_transitions.push_back({
    state_from,
    state_to,
    event,
    on_transition
  });

  m_num_transitions++;
}


void Fsm::add_timed_transition(State* state_from, State* state_to, unsigned long interval, std::function<void(void)> on_transition) {
  
  if (state_from == NULL || state_to == NULL) return;

  const Transition transition = create_transition(state_from, state_to, 0, on_transition);

  m_timed_transitions.push_back({
    transition,
    0,
    interval
  });

  m_num_timed_transitions++;
}

Fsm::Transition Fsm::create_transition(State* state_from, State* state_to, int event, std::function<void(void)> on_transition) {

  Transition t;
  t.state_from = state_from;
  t.state_to = state_to;
  t.event = event;
  t.on_transition = on_transition;

  return t;
}

void Fsm::trigger(int event) {
  if (m_initialized)
    for (int i = 0; i < m_num_transitions; ++i)
      if (m_transitions[i].state_from == m_current_state &&
          m_transitions[i].event == event) {
        Fsm::make_transition(&(m_transitions[i]));
        return;
      }
}

void Fsm::check_timed_transitions() {

  for (int i = 0; i < m_num_timed_transitions; ++i) {

    TimedTransition* transition = &m_timed_transitions[i];
    if (transition->transition.state_from == m_current_state) {
      if (transition->start == 0)
        transition->start = millis();
      else {
        const unsigned long now = millis();
        if (now - transition->start >= transition->interval) {
          Fsm::make_transition(&(transition->transition));
          transition->start = 0;
        }
      }
    }
  }
}

void Fsm::run_machine() {

  if (!m_initialized) {
    m_initialized = true;
    if (m_current_state->on_enter != NULL)
      m_current_state->on_enter();
  }
  
  if (m_current_state->on_state != NULL)
    m_current_state->on_state();
    
  Fsm::check_timed_transitions();
}

void Fsm::make_transition(Transition* transition) {

  if (transition->state_from->on_exit != NULL)
    transition->state_from->on_exit();

  if (transition->on_transition != NULL)
    transition->on_transition();

  if (transition->state_to->on_enter != NULL)
    transition->state_to->on_enter();
  
  m_current_state = transition->state_to;

  unsigned long now = millis();
  for (int i = 0; i < m_num_timed_transitions; ++i) {
    TimedTransition* ttransition = &m_timed_transitions[i];
    if (ttransition->transition.state_from == m_current_state)
      ttransition->start = now;
  }
}