#include "../lib.h"
#include "tests.h"

START_TEST(t_lib_init) {
  initGame();
  ck_assert_int_eq(getGameHasFinished(), false);
  ck_assert_int_eq(getGameOver(), false);
  ck_assert_int_eq(getPause(), false);
  userInput(Terminate, true);
  updateCurrentState();
}
END_TEST

START_TEST(t_lib_provoke_autoshift) {
  initGame();
  ck_assert_int_eq(getGameHasFinished(), false);
  ck_assert_int_eq(getGameOver(), false);
  ck_assert_int_eq(getPause(), false);
  userInput(Start, true);
  updateCurrentState();
  updateCurrentState();
  userInput(Terminate, true);
  updateCurrentState();
}
END_TEST

Suite *ts_lib(void) {
  Suite *s1 = suite_create("ts_lib");
  TCase *t1 = tcase_create("tc_lib");

  suite_add_tcase(s1, t1);
  tcase_add_test(t1, t_lib_init);
  tcase_add_test(t1, t_lib_provoke_autoshift);

  return s1;
}
