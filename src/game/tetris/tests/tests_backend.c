#include "../defines.h"
#include "../matrix.h"
#include "tests.h"

START_TEST(t_backend_init_destroy_overflow) {
  tetris_game_t game = {0};
  ck_assert_int_eq(backend_init_game(&game), false);
  game.game.field[0][3] = 1;
  ck_assert_int_eq(backend_get_overflow(&game), true);
  backend_destroy_game(&game);
}
END_TEST

void matrix_fill_pattern(int **m, const int r, const int c, int q) {
  if (q == 0) return;
  if (q < 0) q = -q;
  for (int i = 0; i < r; ++i) {
    for (int j = 0; j < c; ++j) {
      if ((i * c + j) % q == 0) {
        m[i][j] = 1;
      }
    }
  }
}

bool matrix_assert_pattern(int **m, const int r, const int c, int q) {
  if (q == 0) return false;
  if (q < 0) q = -q;
  bool valid_pattern = true;
  for (int i = 0; valid_pattern && i < r; ++i) {
    for (int j = 0; valid_pattern && j < c; ++j) {
      const bool should_be_filled = (i * c + j) % q == 0;
      const bool actually_filled = m[i][j] > 0;
      if (actually_filled ^ should_be_filled) {
        valid_pattern = false;
      }
    }
  }
  return valid_pattern;
}

START_TEST(t_backend_cut_filled) {
  tetris_game_t game = {0};
  ck_assert_int_eq(backend_init_game(&game), false);
  matrix_fill_pattern(game.game.field + 5, 10, FIELD_WIDTH, 3);
  fill_matrix(game.game.field + 20, FIELD_TOTAL_HEIGHT - 20, FIELD_WIDTH, 1);
  ck_assert_int_eq(
      matrix_assert_pattern(game.game.field + 5, 10, FIELD_WIDTH, 3), true);
  backend_cut_filled_rows(&game);
  ck_assert_int_eq(
      matrix_assert_pattern(game.game.field + 9, 10, FIELD_WIDTH, 3), true);
  ck_assert_int_eq(game.game.score, 1500);
  ck_assert_int_eq(game.game.level, 2);
  fill_matrix(game.game.field + 21, FIELD_TOTAL_HEIGHT - 21, FIELD_WIDTH, 1);
  backend_cut_filled_rows(&game);
  ck_assert_int_eq(game.game.score, 2200);
  ck_assert_int_eq(game.game.level, 3);
  fill_matrix(game.game.field + 22, FIELD_TOTAL_HEIGHT - 22, FIELD_WIDTH, 1);
  backend_cut_filled_rows(&game);
  ck_assert_int_eq(game.game.score, 2500);
  ck_assert_int_eq(game.game.level, 4);
  fill_matrix(game.game.field + 23, FIELD_TOTAL_HEIGHT - 23, FIELD_WIDTH, 1);
  backend_cut_filled_rows(&game);
  ck_assert_int_eq(game.game.score, 2600);
  ck_assert_int_eq(game.game.level, 4);
  backend_destroy_game(&game);
}
END_TEST

START_TEST(t_backend_cut_filled_multiple_through_not_filled) {
  tetris_game_t game = {0};
  ck_assert_int_eq(backend_init_game(&game), false);
  matrix_fill_pattern(game.game.field + 5, 10, FIELD_WIDTH, 3);
  fill_matrix(game.game.field + 20, FIELD_TOTAL_HEIGHT - 20, FIELD_WIDTH, 1);
  fill_matrix(game.game.field + 21, 1, FIELD_WIDTH, 0);
  game.game.field[21][1] = 9;
  game.game.field[21][8] = 8;
  print_matrix(game.game.field, FIELD_TOTAL_HEIGHT, FIELD_WIDTH);
  backend_cut_filled_rows(&game);
  print_matrix(game.game.field, FIELD_TOTAL_HEIGHT, FIELD_WIDTH);
  ck_assert_int_eq(game.game.score, 400);
  ck_assert_int_eq(game.game.level, 0);
  ck_assert_int_eq(
      matrix_assert_pattern(game.game.field + 8, 10, FIELD_WIDTH, 3), true);
  backend_destroy_game(&game);
}
END_TEST

START_TEST(t_backend_setup_new) {
  tetris_game_t game = {0};
  ck_assert_int_eq(backend_init_game(&game), false);
  matrix_fill_pattern(game.game.field + 5, 10, FIELD_WIDTH, 3);
  game.game.score = 10000;
  game.game.level = 10000;

  backend_setup_new_game(&game);

  for (int i = 0; i < FIELD_TOTAL_HEIGHT; ++i) {
    for (int j = 0; j < FIELD_WIDTH; ++j) {
      ck_assert_int_eq(game.game.field[i][j], 0);
    }
  }
  ck_assert_int_eq(game.game.score, 0);
  ck_assert_int_eq(game.game.level, 0);

  bool next_not_empty = false;
  for (int i = 0; !next_not_empty && i < MAX_FIGURE_SIZE; ++i) {
    for (int j = 0; !next_not_empty && j < MAX_FIGURE_SIZE; ++j) {
      if (game.game.next[i][j]) {
        next_not_empty = true;
      }
    }
  }
  ck_assert_int_eq(next_not_empty, true);

  backend_destroy_game(&game);
}
END_TEST

// test is based on assumption that all the patterns
// are aligned with their bottom part without rotation
START_TEST(t_backend_drop) {
  tetris_game_t game = {0};
  ck_assert_int_eq(backend_init_game(&game), false);
  backend_setup_new_game(&game);
  ck_assert_int_eq(backend_spawn_new_figure(&game), false);
  for (int i = 0; i != FIELD_VISIBLE_HEIGHT - 2; ++i) {
    ck_assert_int_eq(backend_drop_current_figure(&game), false);
  }
  ck_assert_int_eq(backend_drop_current_figure(&game), true);
  backend_destroy_game(&game);
}
END_TEST

START_TEST(t_backend_rotate_move) {
  tetris_game_t game = {0};
  ck_assert_int_eq(backend_init_game(&game), false);
  backend_setup_new_game(&game);
  ck_assert_int_eq(backend_spawn_new_figure(&game), false);
  backend_drop_current_figure(&game);
  fill_matrix(game.game.field, FIELD_TOTAL_HEIGHT, FIELD_WIDTH, 0);
  matrix_fill_pattern(game.game.field + FIELD_UPPER_MARGIN, 10, FIELD_WIDTH, 3);
  fill_matrix(game.current_figure.mask, 4, 4, 1);
  backend_rotate_current_figure(&game);
  backend_rotate_current_figure(&game);
  backend_rotate_current_figure(&game);
  backend_rotate_current_figure(&game);
  ck_assert_int_eq(matrix_assert_pattern(game.game.field + FIELD_UPPER_MARGIN,
                                         10, FIELD_WIDTH, 3),
                   true);
  backend_move_left_current_figure(&game);
  ck_assert_int_eq(matrix_assert_pattern(game.game.field + FIELD_UPPER_MARGIN,
                                         10, FIELD_WIDTH, 3),
                   true);
  backend_move_right_current_figure(&game);
  ck_assert_int_eq(matrix_assert_pattern(game.game.field + FIELD_UPPER_MARGIN,
                                         10, FIELD_WIDTH, 3),
                   true);
  backend_destroy_game(&game);
}
END_TEST

Suite *ts_backend(void) {
  Suite *s1 = suite_create("ts_backend");
  TCase *t1 = tcase_create("tc_backend");

  suite_add_tcase(s1, t1);
  tcase_add_test(t1, t_backend_init_destroy_overflow);
  tcase_add_test(t1, t_backend_cut_filled);
  tcase_add_test(t1, t_backend_setup_new);
  tcase_add_test(t1, t_backend_drop);
  tcase_add_test(t1, t_backend_rotate_move);
  tcase_add_test(t1, t_backend_cut_filled_multiple_through_not_filled);

  return s1;
}
