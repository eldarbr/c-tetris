#include "lib.h"

/// @file lib.c
/// @brief Implementation of methods to operate with the tetris game object

#include <stdlib.h>

#include "../../common/time_utils.h"
#include "backend.h"
#include "defines.h"
#include "fsm.h"

/// @brief get ptr to the current game
/// @return ptr to the current game
tetris_game_t *get_current_game(void) {
  static tetris_game_t game;
  return &game;
}

/// @brief get ptr to the user input QUEUE
/// @return ptr to the queue
bool *get_user_input_state_array(void) {
  static bool user_input_state[USERACTIONS_COUNT];
  return user_input_state;
}

/// @brief get the value of an input in the queue
/// @param input user input id
/// @return value from the queue
bool get_user_input_state(const int input) {
  if (input < 0 || input >= USERACTIONS_COUNT) {
    return false;
  }
  return get_user_input_state_array()[input];
}

/// @brief set the value of an input in the queue
/// @param input user input id
/// @param hold new value
void set_user_input_state(const int input, bool hold) {
  get_user_input_state_array()[input] = hold;
}

/// @brief save user input in the queue
/// @param action user input id
/// @param hold user input value
void userInput(UserAction_t action, bool hold) {
  set_user_input_state(action, hold);
}

bool getGameHasFinished(void) { return fsm_get_state() == EXIT; }
bool getGameOver(void) { return fsm_get_state() == GAMEOVER; }
bool getPause(void) { return fsm_get_state() == PAUSE; }

unsigned long get_autoshift_interval_ms(const int level);
unsigned long get_timespec_diff_ms(const struct timespec *later,
                                   const struct timespec *earlier);

/// @brief update game state. autoshift if it is time to, otherwise apply user
/// input from the queue
/// @param
void handle_game_update(void) {
  static struct timespec previous_atoshift_sig;

  fsm_input_t signal = NO_INPUT;
  if (fsm_is_autoshift_available() &&
      get_is_time_to_operate_ms_diff(
          &previous_atoshift_sig,
          get_autoshift_interval_ms(get_current_game()->game.level))) {
    signal = AUTOSHIFT_SIG;
  }
  if (!signal) {
    for (int i = 0; !signal && i < USERACTIONS_COUNT; ++i) {
      if (get_user_input_state(i)) {
        signal = fsm_get_signal(i);
        set_user_input_state(i, false);
      }
    }
  }
  fsm_apply_input(signal, get_current_game());
}

/// @brief get ammount of mseconds that should pass between the autoshifts
/// @param level game level
/// @return interval between autoshifts
unsigned long get_autoshift_interval_ms(const int level) {
  unsigned long interval = 1000;
  if (level > 0 && level <= 10) {
    interval -= 90 * level;
  }
  return interval;
}

/// @brief handle game update and return the updated state of the game
/// @return updated game state
GameInfo_t updateCurrentState(void) {
  handle_game_update();
  return get_current_game()->game;
}

/// @brief initialize the FSM
/// @param
void initGame(void) {
  srand(time(NULL));
  fsm_apply_input(NO_INPUT, get_current_game());
}
