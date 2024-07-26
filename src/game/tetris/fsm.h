#ifndef TETRIS_FSM
#define TETRIS_FSM

/// @file fsm.h
/// @brief Declaration of types and methods to operate with the FSM

#include "backend.h"
#include "lib.h"

typedef enum {
  PRESTART = 0,
  START,
  SPAWNING,
  IDLE,
  MOVING,
  PAUSE,
  AUTOSHIFTING,
  OVERFLOWCONTROL,
  ROWCUTTING,
  GAMEOVER,
  PREEXIT,
  EXIT
} tetris_state_t;

typedef enum {
  NO_INPUT = 0,
  START_BTN,
  PAUSE_BTN,
  EXIT_BTN,
  MOVE_DOWN,
  MOVE_LEFT,
  MOVE_RIGHT,
  ROTATE_BTN,
  AUTOSHIFT_SIG
} fsm_input_t;

fsm_input_t fsm_get_signal(UserAction_t user_input);
void fsm_apply_input(fsm_input_t, tetris_game_t *);
tetris_state_t fsm_get_state(void);
bool fsm_is_autoshift_available(void);

#endif
