#ifndef TETRIS_BACKEND
#define TETRIS_BACKEND

/// @file backend.h
/// @brief Declaration of functions to move figures

#include <stdbool.h>

#include "lib.h"

typedef struct {
  int r;
  int c;
} coords_t;

typedef struct {
  int **mask;
  coords_t position;
} figure_t;

typedef struct {
  GameInfo_t game;
  figure_t current_figure;
} tetris_game_t;

bool backend_init_game(tetris_game_t *);
void backend_destroy_game(tetris_game_t *);

void backend_setup_new_game(tetris_game_t *);
void backend_cut_filled_rows(tetris_game_t *);
bool backend_spawn_new_figure(tetris_game_t *);
bool backend_drop_current_figure(tetris_game_t *);
void backend_rotate_current_figure(tetris_game_t *);
void backend_move_left_current_figure(tetris_game_t *);
void backend_move_right_current_figure(tetris_game_t *);
bool backend_get_overflow(const tetris_game_t *);

#endif
