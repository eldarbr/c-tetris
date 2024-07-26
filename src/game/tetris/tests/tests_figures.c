#include <stdlib.h>
#include <time.h>

#include "../matrix.h"
#include "tests.h"

START_TEST(t_figures_randomness) {
  srand(time(NULL));
  int hits[ALLOWED_FIGURES_COUNT] = {0};
  int **m = calloc_matrix(4, 4);
  for (int i = 0; i < 1000000; ++i) {
    const int next_figure_id = rand() % (ALLOWED_FIGURES_COUNT);
    fill_figure_by_id(m, next_figure_id);
    ++hits[next_figure_id];
  }
  free_matrix(m);
  int chances[ALLOWED_FIGURES_COUNT] = {0};
  for (int i = 0; i < ALLOWED_FIGURES_COUNT; ++i) {
    chances[i] = hits[i] / 10000;
    if (i) {
      ck_assert_int_eq(chances[i], chances[i - 1]);
    }
  }
}
END_TEST

Suite *ts_figures(void) {
  Suite *s1 = suite_create("ts_figures");
  TCase *t1 = tcase_create("tc_figures");

  suite_add_tcase(s1, t1);
  tcase_add_test(t1, t_figures_randomness);

  return s1;
}
