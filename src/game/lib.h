#ifndef GAME_LIB
#define GAME_LIB

/// @file lib.h
/// @brief Declaration of methods and types to operate with a game object

#include <stdbool.h>

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;
#define USERACTIONS_COUNT 8

typedef struct {
  int **field;
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

void userInput(UserAction_t action, bool hold);
GameInfo_t updateCurrentState(void);

#endif
