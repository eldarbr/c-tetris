#include "fsm.h"

/// @file fsm.c
/// @brief Implementation of types and methods to operate with the FSM

#include "backend.h"
#include "lib.h"

/// @brief transitions from the PRESTART state
/// @param state current state ptr
/// @param game current game ptr
void fsm_prestart_transition(tetris_state_t *const state,
                             tetris_game_t *const game) {
  if (!state || !game) return;
  if (backend_init_game(game)) {
    *state = EXIT;
  } else {
    *state = START;
  }
}

/// @brief transitions from the START state
/// @param input user input ptr
/// @param state current state ptr
/// @param game current game ptr
void fsm_start_transition(fsm_input_t *const input, tetris_state_t *const state,
                          tetris_game_t *const game) {
  if (!state || !input) return;
  switch (*input) {
    case START_BTN:
      *state = SPAWNING;
      backend_setup_new_game(game);
      break;
    case EXIT_BTN:
      *state = PREEXIT;
      break;
    default:
      *state = START;
      break;
  }
  *input = NO_INPUT;
}

/// @brief transitions from the SPAWNING state
/// @param state current state ptr
/// @param game current game ptr
void fsm_spawning_transition(tetris_state_t *const state,
                             tetris_game_t *const game) {
  if (!state) return;
  if (backend_spawn_new_figure(game)) {
    *state = GAMEOVER;
  } else {
    *state = IDLE;
  }
}

/// @brief transitions from the IDLE state
/// @param input user input ptr
/// @param state current state ptr
void fsm_idle_transition(fsm_input_t *const input,
                         tetris_state_t *const state) {
  if (!state || !input) return;
  switch (*input) {
    case EXIT_BTN:
      *state = PREEXIT;
      *input = NO_INPUT;
      break;
    case PAUSE_BTN:
      *state = PAUSE;
      *input = NO_INPUT;
      break;
    case MOVE_LEFT:
    case MOVE_RIGHT:
    case MOVE_DOWN:
    case ROTATE_BTN:
      *state = MOVING;
      break;
    case AUTOSHIFT_SIG:
      *state = AUTOSHIFTING;
      *input = NO_INPUT;
      break;
    default:
      *state = IDLE;
      *input = NO_INPUT;
      break;
  }
}

/// @brief transitions from the AUTOSHIFTING state
/// @param state current state ptr
/// @param game current game ptr
void fsm_autoshifting_transition(tetris_state_t *const state,
                                 tetris_game_t *const game) {
  if (!state) return;
  if (!backend_drop_current_figure(game)) {
    *state = IDLE;
  } else {
    *state = OVERFLOWCONTROL;
  }
}

/// @brief transitions from the MOVING state
/// @param input user input ptr
/// @param state current state ptr
/// @param game current game ptr
void fsm_moving_transition(fsm_input_t *const input,
                           tetris_state_t *const state,
                           tetris_game_t *const game) {
  if (!state || !input) return;
  switch (*input) {
    case MOVE_LEFT:
      backend_move_left_current_figure(game);
      break;
    case MOVE_RIGHT:
      backend_move_right_current_figure(game);
      break;
    case ROTATE_BTN:
      backend_rotate_current_figure(game);
      break;
    case MOVE_DOWN:
      backend_drop_current_figure(game);
      break;
    default:
      break;
  }
  *state = IDLE;
  *input = NO_INPUT;
}

/// @brief transitions from the PAUSE state
/// @param input user input ptr
/// @param state current state ptr
void fsm_pause_transition(fsm_input_t *const input,
                          tetris_state_t *const state) {
  if (!state || !input) return;
  switch (*input) {
    case PAUSE_BTN:
      *state = IDLE;
      break;
    case EXIT_BTN:
      *state = PREEXIT;
      break;
    default:
      *state = PAUSE;
      break;
  }
  *input = NO_INPUT;
}

/// @brief transitions from the OVERFLOWCONTROL state
/// @param state current state ptr
/// @param game current game ptr
void fsm_overflowcontrol_transition(tetris_state_t *const state,
                                    tetris_game_t *const game) {
  if (!state) return;
  if (backend_get_overflow(game)) {
    *state = GAMEOVER;
  } else {
    *state = ROWCUTTING;
  }
}

/// @brief transitions from the ROWCUTTING state
/// @param state current state ptr
/// @param game current game ptr
void fsm_rowcutting_transition(tetris_state_t *const state,
                               tetris_game_t *const game) {
  if (!state) return;
  backend_cut_filled_rows(game);
  *state = SPAWNING;
}

/// @brief transitions from the GAMEOVER state
/// @param input user input ptr
/// @param state current state ptr
/// @param game current game ptr
void fsm_gameover_transition(fsm_input_t *const input,
                             tetris_state_t *const state,
                             tetris_game_t *const game) {
  if (!state || !input) return;
  switch (*input) {
    case EXIT_BTN:
      *state = PREEXIT;
      break;
    case START_BTN:
      *state = SPAWNING;
      backend_setup_new_game(game);
      break;
    default:
      *state = GAMEOVER;
      break;
  }
  *input = NO_INPUT;
}

/// @brief transitions from the PREEXIT state
/// @param state current state ptr
/// @param game current game ptr
void fsm_preexit_transition(tetris_state_t *const state,
                            tetris_game_t *const game) {
  if (!state) return;
  backend_destroy_game(game);
  *state = EXIT;
}

/// @brief get ptr to the state object
/// @return ptr to the state object
tetris_state_t *get_current_state(void) {
  static tetris_state_t state;
  return &state;
}

/// @brief get the value of the state object
/// @return value of the state object
tetris_state_t fsm_get_state(void) { return *get_current_state(); }

/// @brief get if the autoshift is available in the current state
/// @return true if autoshift is available
bool fsm_is_autoshift_available(void) {
  const tetris_state_t state = fsm_get_state();
  bool available = false;
  if (state == IDLE) available = true;
  return available;
}

/// @brief Apply user input at the current state of the FSM
/// @param inp user input value
/// @param game current game
void fsm_apply_input(fsm_input_t inp, tetris_game_t *const game) {
  tetris_state_t *state = get_current_state();
  bool at_least_once = false;
  while (inp != NO_INPUT || !at_least_once) {
    switch (*state) {
      case START:
        fsm_start_transition(&inp, state, game);
        break;
      case SPAWNING:
        fsm_spawning_transition(state, game);
        break;
      case IDLE:
        fsm_idle_transition(&inp, state);
        break;
      case AUTOSHIFTING:
        fsm_autoshifting_transition(state, game);
        break;
      case MOVING:
        fsm_moving_transition(&inp, state, game);
        break;
      case PAUSE:
        fsm_pause_transition(&inp, state);
        break;
      case OVERFLOWCONTROL:
        fsm_overflowcontrol_transition(state, game);
        break;
      case ROWCUTTING:
        fsm_rowcutting_transition(state, game);
        break;
      case GAMEOVER:
        fsm_gameover_transition(&inp, state, game);
        break;
      case PREEXIT:
        fsm_preexit_transition(state, game);
        break;
      case PRESTART:
        fsm_prestart_transition(state, game);
        break;
      default:
        break;
    }
    at_least_once = true;
  }
}

/// @brief translate user input to the fsm signal
/// @param user_input user input value
/// @return fsm signal value
fsm_input_t fsm_get_signal(UserAction_t user_input) {
  fsm_input_t input = NO_INPUT;
  switch (user_input) {
    case Start:
      input = START_BTN;
      break;
    case Pause:
      input = PAUSE_BTN;
      break;
    case Terminate:
      input = EXIT_BTN;
      break;
    case Left:
      input = MOVE_LEFT;
      break;
    case Right:
      input = MOVE_RIGHT;
      break;
    case Down:
      input = MOVE_DOWN;
      break;
    case Action:
      input = ROTATE_BTN;
      break;
    default:
      break;
  }
  return input;
}
