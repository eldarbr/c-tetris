#include "backend.h"

/// @file backend.c
/// @brief Implementation of functions to move figures

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "defines.h"
#include "figures.h"
#include "matrix.h"

#define SAVE_FILE_PATH "/tmp/.tetris_high_score"

int generate_next_figure(tetris_game_t *game);
void load_high_score(tetris_game_t *game);
void save_high_score(tetris_game_t *game);

/// @brief Clear game field and score, prepare 'next figure' for the game start
/// @param game ptr to a intialized current game
void backend_setup_new_game(tetris_game_t *game) {
  if (!game) return;
  fill_matrix(game->game.field, FIELD_TOTAL_HEIGHT, FIELD_WIDTH, 0);
  fill_matrix(game->game.next, MAX_FIGURE_SIZE, MAX_FIGURE_SIZE, 0);
  fill_matrix(game->current_figure.mask, MAX_FIGURE_SIZE, MAX_FIGURE_SIZE, 0);
  game->game.score = 0;
  game->game.level = 0;
  load_high_score(game);
  generate_next_figure(game);
}

/// @brief Acquire resources for game field and figures
/// @param game ptr where to save the game
/// @return true on malloc error
bool backend_init_game(tetris_game_t *game) {
  if (!game) return true;
  bool error = false;
  game->game.field = calloc_matrix(FIELD_TOTAL_HEIGHT, FIELD_WIDTH);
  error = game->game.field == NULL;
  if (!error) {
    game->game.next = calloc_matrix(MAX_FIGURE_SIZE, MAX_FIGURE_SIZE);
    error = game->game.next == NULL;
  }
  if (!error) {
    game->current_figure.mask = calloc_matrix(MAX_FIGURE_SIZE, MAX_FIGURE_SIZE);
    error = game->current_figure.mask == NULL;
  }
  if (!error) {
    load_high_score(game);
  }
  if (error) {
    backend_destroy_game(game);
  }
  return error;
}

#define spawn_position_r FIELD_UPPER_MARGIN - 1
#define spawn_position_c FIELD_WIDTH / 2 - MAX_FIGURE_SIZE / 2
/// @brief Locks the current figure, places the next figure on the field
/// @param game ptr to current game
/// @return true if there was a collision of the new figure with anything
bool swap_current_to_next_figure(tetris_game_t *game) {
  if (!game) return false;
  bool collision = false;
  for (int i = 0; i != MAX_FIGURE_SIZE; ++i) {
    for (int j = 0; j != MAX_FIGURE_SIZE; ++j) {
      const int field_value =
          game->game.field[spawn_position_r + i][spawn_position_c + j];
      const int mask_value = game->game.next[i][j];
      if (field_value && mask_value) {
        collision = true;
      }
      if (mask_value) {
        game->game.field[spawn_position_r + i][spawn_position_c + j] =
            mask_value;
      }
      game->current_figure.mask[i][j] = mask_value;
    }
  }
  game->current_figure.position.r = spawn_position_r;
  game->current_figure.position.c = spawn_position_c;
  return collision;
}

/// @brief Free game resources
/// @param game ptr to the game
void backend_destroy_game(tetris_game_t *game) {
  if (!game) return;
  if (game->game.field) {
    free_matrix(game->game.field);
    game->game.field = NULL;
  }
  if (game->game.next) {
    free_matrix(game->game.next);
    game->game.next = NULL;
  }
  if (game->current_figure.mask) {
    free_matrix(game->current_figure.mask);
    game->current_figure.mask = NULL;
  }
}

/// @brief Place a random figure in the 'next figure' matrix
/// @param game current game
/// @return new figure id
int generate_next_figure(tetris_game_t *game) {
  if (!game) return 0;
  const int next_figure_id = rand() % (ALLOWED_FIGURES_COUNT);
  fill_figure_by_id(game->game.next, next_figure_id);
  return next_figure_id;
}

void plus_score(tetris_game_t *game, const int cutted_rows_count);

/// @brief Remove rows that are full, shifting down the upper rows. Updates user
/// score
/// @param game current game
void backend_cut_filled_rows(tetris_game_t *game) {
  if (!game) return;
  int pivot = FIELD_UPPER_MARGIN;
  while (pivot != FIELD_TOTAL_HEIGHT) {
    while (pivot < FIELD_TOTAL_HEIGHT &&
           !get_is_a_filled_row((const int **)(game->game.field), pivot,
                                FIELD_WIDTH)) {
      ++pivot;
    }
    int filled_rows_count = 0;
    while (pivot + filled_rows_count < FIELD_TOTAL_HEIGHT &&
           get_is_a_filled_row((const int **)(game->game.field),
                               pivot + filled_rows_count, FIELD_WIDTH)) {
      ++filled_rows_count;
    }
    if (filled_rows_count) {
      shift_down_rows(game->game.field, pivot + filled_rows_count - 1,
                      filled_rows_count, FIELD_WIDTH);
      plus_score(game, filled_rows_count);
    }
  }
}

/// @brief Updates user score depending on the ammount of full rows that were
/// delete
/// @param game current game
/// @param cutted_rows_count ammount of deleted rows
void plus_score(tetris_game_t *game, const int cutted_rows_count) {
  if (!game) return;
  int score_delta = 0;
  switch (cutted_rows_count) {
    case 1:
      score_delta = 100;
      break;
    case 2:
      score_delta = 300;
      break;
    case 3:
      score_delta = 700;
      break;
    case 4:
      score_delta = 1500;
      break;
    default:
      break;
  }
  game->game.score += score_delta;
  game->game.level = game->game.score / 600;
  if (score_delta && game->game.score > game->game.high_score) {
    save_high_score(game);
  }
  if (game->game.level > 10) {
    game->game.level = 10;
  }
}

void edit_current_figure(tetris_game_t *game, const figure_t *new_figure);
bool check_new_old_figure_exclusive_collision(const tetris_game_t *const game,
                                              const figure_t *edited_figure);
bool check_walls_collision(const figure_t *);

/// @brief Shift down one step the current figure with collision
/// @param game current game
/// @return false if the shift was successful, meaning there was no collision
bool backend_drop_current_figure(tetris_game_t *game) {
  if (!game) return true;
  figure_t edited = {0};
  edited.mask = calloc_matrix(MAX_FIGURE_SIZE, MAX_FIGURE_SIZE);
  memcpy(edited.mask[0], game->current_figure.mask[0],
         sizeof(int) * MAX_FIGURE_SIZE * MAX_FIGURE_SIZE);
  edited.position.r = game->current_figure.position.r + 1;
  edited.position.c = game->current_figure.position.c;
  const bool collision =
      check_new_old_figure_exclusive_collision(game, &edited) ||
      check_walls_collision(&edited);
  if (!collision) {
    edit_current_figure(game, &edited);
  }
  free_matrix(edited.mask);
  return collision;
}

/// @brief Check if the full cells of the figure mask are out of bounds
/// @param fig figure to check
/// @return false if the figure is inside the field bounds
bool check_walls_collision(const figure_t *fig) {
  bool collision = false;
  for (int r = 0; !collision && r != MAX_FIGURE_SIZE; ++r) {
    for (int c = 0; !collision && c != MAX_FIGURE_SIZE; ++c) {
      const int mask_value = fig->mask[r][c];
      if (mask_value) {
        const int absolute_r = r + fig->position.r;
        const int absolute_c = c + fig->position.c;
        collision = absolute_r < 0 || absolute_r >= FIELD_TOTAL_HEIGHT ||
                    absolute_c < 0 || absolute_c >= FIELD_WIDTH;
      }
    }
  }
  return collision;
}

/// @brief Remove from the field the old figure, replace it with the new one. No
/// collision control
/// @param game current game
/// @param edited_figure figure to replace with
void edit_current_figure(tetris_game_t *const game,
                         const figure_t *edited_figure) {
  if (!game) return;
  // remove old figure from the field
  for (int r = 0; r != MAX_FIGURE_SIZE; ++r) {
    for (int c = 0; c != MAX_FIGURE_SIZE; ++c) {
      const int absolute_r = r + game->current_figure.position.r;
      const int absolute_c = c + game->current_figure.position.c;
      const int mask_value = game->current_figure.mask[r][c];
      if (absolute_c >= 0 && absolute_c < FIELD_WIDTH && absolute_r >= 0 &&
          absolute_r < FIELD_TOTAL_HEIGHT && mask_value) {
        game->game.field[absolute_r][absolute_c] = 0;
      }
    }
  }
  // place new figure
  for (int r = 0; r != MAX_FIGURE_SIZE; ++r) {
    for (int c = 0; c != MAX_FIGURE_SIZE; ++c) {
      const int absolute_r = r + edited_figure->position.r;
      const int absolute_c = c + edited_figure->position.c;
      const int mask_value = edited_figure->mask[r][c];
      if (absolute_c >= 0 && absolute_c < FIELD_WIDTH && absolute_r >= 0 &&
          absolute_r < FIELD_TOTAL_HEIGHT && mask_value) {
        game->game.field[absolute_r][absolute_c] = mask_value;
      }
      game->current_figure.mask[r][c] = edited_figure->mask[r][c];
    }
  }
  game->current_figure.position.r = edited_figure->position.r;
  game->current_figure.position.c = edited_figure->position.c;
}

int get_field_value_no_current_figure(const tetris_game_t *const game,
                                      const int r, const int c);

/// @brief Check if the new figure collides with any full cells of the field
/// without the current figure
/// @param game curernt game
/// @param new_figure new figure
/// @return false if no collision
bool check_new_old_figure_exclusive_collision(const tetris_game_t *const game,
                                              const figure_t *new_figure) {
  bool collision = false;
  for (int new_r = 0; !collision && new_r != MAX_FIGURE_SIZE; ++new_r) {
    for (int new_c = 0; !collision && new_c != MAX_FIGURE_SIZE; ++new_c) {
      const int absolute_new_r = new_r + new_figure->position.r;
      const int absolute_new_c = new_c + new_figure->position.c;
      const int new_field_value = new_figure->mask[new_r][new_c];
      const int field_value_no_old = get_field_value_no_current_figure(
          game, absolute_new_r, absolute_new_c);
      if (new_field_value &&
          (absolute_new_c < 0 || absolute_new_c >= FIELD_WIDTH ||
           absolute_new_r < 0 || absolute_new_r >= FIELD_TOTAL_HEIGHT)) {
        collision = true;
      } else if (new_field_value && field_value_no_old) {
        collision = true;
      }
    }
  }
  return collision;
}

/// @brief Get value of a cell in the field without the current figure
/// @param game current game
/// @param r cell row
/// @param c cell column
/// @return cell value
int get_field_value_no_current_figure(const tetris_game_t *const game,
                                      const int r, const int c) {
  if (!game || r < 0 || r >= FIELD_TOTAL_HEIGHT || c < 0 || c >= FIELD_WIDTH)
    return 0;
  int field_value = game->game.field[r][c];
  const int inside_mask_r = r - game->current_figure.position.r;
  const int inside_mask_c = c - game->current_figure.position.c;
  if (inside_mask_r >= 0 && inside_mask_r < MAX_FIGURE_SIZE &&
      inside_mask_c >= 0 && inside_mask_c < MAX_FIGURE_SIZE) {
    field_value -= game->current_figure.mask[inside_mask_r][inside_mask_c];
  }
  return field_value;
}

/// @brief Lock the current figure, place the next figure on the field making it
/// the new current, generate next figure
/// @param game current game
/// @return
bool backend_spawn_new_figure(tetris_game_t *const game) {
  if (!game) return false;
  bool collision = false;
  if (swap_current_to_next_figure(game)) {
    collision = true;
  }
  generate_next_figure(game);
  return collision;
}

/// @brief Replace the current figure with its rotated version, if the operation
/// is possible = there will be no collision
/// @param game current game
void backend_rotate_current_figure(tetris_game_t *const game) {
  if (!game) return;
  figure_t edited = {0};
  edited.mask = calloc_matrix(MAX_FIGURE_SIZE, MAX_FIGURE_SIZE);
  for (int old_r = 0; old_r != MAX_FIGURE_SIZE; ++old_r) {
    for (int old_c = 0; old_c != MAX_FIGURE_SIZE; ++old_c) {
      const int new_r = old_c;
      const int new_c = MAX_FIGURE_SIZE - old_r - 1;
      edited.mask[new_r][new_c] = game->current_figure.mask[old_r][old_c];
    }
  }
  edited.position.r = game->current_figure.position.r;
  edited.position.c = game->current_figure.position.c;
  const bool collision =
      check_new_old_figure_exclusive_collision(game, &edited);
  if (!collision) {
    edit_current_figure(game, &edited);
  }
  free_matrix(edited.mask);
  return;
}

/// @brief Replace the current figure with its moved left version, if the
/// operation is possible = there will be no collision
/// @param game current game
void backend_move_left_current_figure(tetris_game_t *const game) {
  if (!game) return;
  figure_t edited = {0};
  edited.mask = calloc_matrix(MAX_FIGURE_SIZE, MAX_FIGURE_SIZE);
  memcpy(edited.mask[0], game->current_figure.mask[0],
         sizeof(int) * MAX_FIGURE_SIZE * MAX_FIGURE_SIZE);
  edited.position.r = game->current_figure.position.r;
  edited.position.c = game->current_figure.position.c - 1;
  const bool collision =
      check_new_old_figure_exclusive_collision(game, &edited);
  if (!collision) {
    edit_current_figure(game, &edited);
  }
  free_matrix(edited.mask);
  return;
}

/// @brief Replace the current figure with its moved right version, if the
/// operation is possible = there will be no collision
/// @param game current game
void backend_move_right_current_figure(tetris_game_t *const game) {
  if (!game) return;
  figure_t edited = {0};
  edited.mask = calloc_matrix(MAX_FIGURE_SIZE, MAX_FIGURE_SIZE);
  memcpy(edited.mask[0], game->current_figure.mask[0],
         sizeof(int) * MAX_FIGURE_SIZE * MAX_FIGURE_SIZE);
  edited.position.r = game->current_figure.position.r;
  edited.position.c = game->current_figure.position.c + 1;
  const bool collision =
      check_new_old_figure_exclusive_collision(game, &edited);
  if (!collision) {
    edit_current_figure(game, &edited);
  }
  free_matrix(edited.mask);
  return;
}

/// @brief check if any of the filled cells are outside of the visible field
/// @param game current game
/// @return false if no cells are outside
bool backend_get_overflow(const tetris_game_t *const game) {
  if (!game) return false;
  bool overflow = false;
  for (int r = 0; !overflow && r < FIELD_UPPER_MARGIN; ++r) {
    for (int c = 0; !overflow && c < FIELD_WIDTH; ++c) {
      if (game->game.field[r][c]) {
        overflow = true;
      }
    }
  }
  return overflow;
}

/// @brief load high score from the SAVE_FILE_PATH file
/// @param game current game
void load_high_score(tetris_game_t *game) {
  FILE *high_score_f = fopen(SAVE_FILE_PATH, "r");
  if (high_score_f) {
    int high_score = 0;
    if (fscanf(high_score_f, "%d", &high_score)) {
      game->game.high_score = high_score;
    }
    fclose(high_score_f);
  }
}

/// @brief save the high score to the SAVE_FILE_PATH file
/// @param game current game
void save_high_score(tetris_game_t *game) {
  FILE *high_score_f = fopen(SAVE_FILE_PATH, "w");
  if (high_score_f) {
    fprintf(high_score_f, "%d", game->game.score);
    fclose(high_score_f);
  }
}
