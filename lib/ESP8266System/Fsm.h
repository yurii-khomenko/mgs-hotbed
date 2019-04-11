#ifndef FSM_H
#define FSM_H

#include <Arduino.h>

struct State
{
  State(std::function<void(void)> on_enter, std::function<void(void)> on_state, std::function<void(void)> on_exit);
  std::function<void(void)> on_enter;
  std::function<void(void)> on_state;
  std::function<void(void)> on_exit;
};

class Fsm
{
public:
  Fsm(State* initial_state);
  ~Fsm();

  void add_transition(State* state_from, State* state_to, int event, std::function<void(void)> on_transition);
  void add_timed_transition(State* state_from, State* state_to, unsigned long interval, std::function<void(void)> on_transition);

  void check_timed_transitions();

  void trigger(int event);
  void run_machine();

private:
  struct Transition
  {
    State* state_from;
    State* state_to;
    int event;
    std::function<void(void)> on_transition;
  };
  struct TimedTransition
  {
    Transition transition;
    unsigned long start;
    unsigned long interval;
  };

  Transition create_transition(State* state_from, State* state_to, int event, std::function<void(void)> on_transition);
  // Transition create_transition(State* state_from, State* state_to, int event);

  void make_transition(Transition *transition);

private:
  State* m_current_state;
  std::vector<Transition> m_transitions;
  int m_num_transitions;

  std::vector<TimedTransition> m_timed_transitions;
  int m_num_timed_transitions;
  bool m_initialized;
};

#endif