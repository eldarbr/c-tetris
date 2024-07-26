#include "tests.h"

int srun_all(Suite *ts);

int main(void) {
  int ftc = 0;

  Suite *s1 = ts_backend();
  Suite *s2 = ts_lib();
  Suite *s3 = ts_figures();
  Suite *s4 = ts_fsm();

  ftc += srun_all(s1);
  ftc += srun_all(s2);
  ftc += srun_all(s3);
  ftc += srun_all(s4);

  return ftc;
}

int srun_all(Suite *ts) {
  SRunner *r = srunner_create(ts);
  srunner_run_all(r, CK_VERBOSE);
  int failed = srunner_ntests_failed(r);
  srunner_free(r);
  return failed;
}
