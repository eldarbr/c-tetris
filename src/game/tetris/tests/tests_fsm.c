#include <time.h>

#include "../lib.h"
#include "../matrix.h"
#include "tests.h"

START_TEST(t_fsm_init_to_exit) {
  tetris_game_t g = {0};
  fsm_apply_input(NO_INPUT, &g);
  fsm_apply_input(fsm_get_signal(Terminate), &g);
  ck_assert_int_eq(fsm_get_state(), PREEXIT);
  fsm_apply_input(NO_INPUT, &g);
  ck_assert_int_eq(fsm_get_state(), EXIT);
}
END_TEST

START_TEST(t_fsm_start_to_spawn_to_idle_to_pause_to_exit) {
  tetris_game_t g = {0};
  fsm_apply_input(NO_INPUT, &g);
  fsm_apply_input(fsm_get_signal(Start), &g);
  ck_assert_int_eq(fsm_get_state(), SPAWNING);
  fsm_apply_input(fsm_get_signal(Pause), &g);
  ck_assert_int_eq(fsm_get_state(), PAUSE);
  fsm_apply_input(fsm_get_signal(Terminate), &g);
  fsm_apply_input(NO_INPUT, &g);
}
END_TEST

START_TEST(t_fsm_start_to_spawn_to_idle_to_moves) {
  tetris_game_t g = {0};
  fsm_apply_input(NO_INPUT, &g);
  fsm_apply_input(fsm_get_signal(Start), &g);
  ck_assert_int_eq(fsm_get_state(), SPAWNING);
  fsm_apply_input(fsm_get_signal(Left), &g);
  ck_assert_int_eq(fsm_get_state(), IDLE);
  fsm_apply_input(fsm_get_signal(Right), &g);
  ck_assert_int_eq(fsm_get_state(), IDLE);
  fsm_apply_input(fsm_get_signal(Down), &g);
  ck_assert_int_eq(fsm_get_state(), IDLE);
  fsm_apply_input(fsm_get_signal(Action), &g);
  ck_assert_int_eq(fsm_get_state(), IDLE);
  fsm_apply_input(fsm_get_signal(Terminate), &g);
  fsm_apply_input(NO_INPUT, &g);
}
END_TEST

START_TEST(t_fsm_start_to_spawn_to_idle_to_autoshift_to_idle) {
  tetris_game_t g = {0};
  fsm_apply_input(NO_INPUT, &g);
  fsm_apply_input(fsm_get_signal(Start), &g);
  ck_assert_int_eq(fsm_get_state(), SPAWNING);
  fsm_apply_input(AUTOSHIFT_SIG, &g);
  ck_assert_int_eq(fsm_get_state(), AUTOSHIFTING);
  fsm_apply_input(NO_INPUT, &g);
  ck_assert_int_eq(fsm_get_state(), IDLE);
  fsm_apply_input(fsm_get_signal(Terminate), &g);
  fsm_apply_input(NO_INPUT, &g);
}
END_TEST

START_TEST(t_fsm_start_to_spawn_to_idle_to_bottom_to_autoshift_to_overflow) {
  tetris_game_t g = {0};
  fsm_apply_input(NO_INPUT, &g);
  fsm_apply_input(fsm_get_signal(Start), &g);
  ck_assert_int_eq(fsm_get_state(), SPAWNING);
  for (int i = 0; i != FIELD_VISIBLE_HEIGHT - 2; ++i) {
    fsm_apply_input(fsm_get_signal(Down), &g);
  }
  fsm_apply_input(AUTOSHIFT_SIG, &g);
  ck_assert_int_eq(fsm_get_state(), AUTOSHIFTING);
  fsm_apply_input(NO_INPUT, &g);
  ck_assert_int_eq(fsm_get_state(), OVERFLOWCONTROL);
  fsm_apply_input(NO_INPUT, &g);
  ck_assert_int_eq(fsm_get_state(), ROWCUTTING);
  fsm_apply_input(NO_INPUT, &g);
  ck_assert_int_eq(fsm_get_state(), SPAWNING);
  fsm_apply_input(NO_INPUT, &g);
  ck_assert_int_eq(fsm_get_state(), IDLE);
  fsm_apply_input(fsm_get_signal(Terminate), &g);
  fsm_apply_input(NO_INPUT, &g);
}
END_TEST

START_TEST(t_fsm_start_to_spawn_to_idle_to_gameover_to_exit) {
  tetris_game_t g = {0};
  fsm_apply_input(NO_INPUT, &g);
  fsm_apply_input(fsm_get_signal(Start), &g);
  ck_assert_int_eq(fsm_get_state(), SPAWNING);
  fsm_apply_input(NO_INPUT, &g);
  ck_assert_int_eq(fsm_get_state(), IDLE);

  fill_matrix(g.game.field, FIELD_TOTAL_HEIGHT, FIELD_WIDTH, 1);
  fsm_apply_input(AUTOSHIFT_SIG, &g);
  ck_assert_int_eq(fsm_get_state(), AUTOSHIFTING);
  fsm_apply_input(NO_INPUT, &g);
  ck_assert_int_eq(fsm_get_state(), OVERFLOWCONTROL);
  fsm_apply_input(NO_INPUT, &g);
  ck_assert_int_eq(fsm_get_state(), GAMEOVER);
  fsm_apply_input(NO_INPUT, &g);
  ck_assert_int_eq(fsm_get_state(), GAMEOVER);
  fsm_apply_input(fsm_get_signal(Terminate), &g);
  ck_assert_int_eq(fsm_get_state(), PREEXIT);
  fsm_apply_input(NO_INPUT, &g);
  ck_assert_int_eq(fsm_get_state(), EXIT);
}
END_TEST

START_TEST(t_fsm_autoshift_availability) {
  tetris_game_t g = {0};
  fsm_apply_input(NO_INPUT, &g);
  fsm_apply_input(fsm_get_signal(Start), &g);
  ck_assert_int_eq(fsm_get_state(), SPAWNING);
  ck_assert_int_eq(fsm_is_autoshift_available(), false);
  fsm_apply_input(NO_INPUT, &g);
  ck_assert_int_eq(fsm_get_state(), IDLE);
  ck_assert_int_eq(fsm_is_autoshift_available(), true);
  fsm_apply_input(fsm_get_signal(Terminate), &g);
  fsm_apply_input(NO_INPUT, &g);
}
END_TEST

Suite *ts_fsm(void) {
  Suite *s1 = suite_create("ts_fsm");
  TCase *t1 = tcase_create("tc_fsm");

  suite_add_tcase(s1, t1);
  tcase_add_test(t1, t_fsm_init_to_exit);
  tcase_add_test(t1, t_fsm_start_to_spawn_to_idle_to_pause_to_exit);
  tcase_add_test(t1, t_fsm_start_to_spawn_to_idle_to_moves);
  tcase_add_test(t1, t_fsm_start_to_spawn_to_idle_to_autoshift_to_idle);
  tcase_add_test(
      t1, t_fsm_start_to_spawn_to_idle_to_bottom_to_autoshift_to_overflow);
  tcase_add_test(t1, t_fsm_start_to_spawn_to_idle_to_gameover_to_exit);
  tcase_add_test(t1, t_fsm_autoshift_availability);

  return s1;
}
