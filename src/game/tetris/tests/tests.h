#ifndef TETRIS_LIB_TESTS
#define TETRIS_LIB_TESTS

#include <check.h>

#include "../backend.h"
#include "../defines.h"
#include "../figures.h"
#include "../fsm.h"
#include "../lib.h"

Suite *ts_backend(void);
Suite *ts_lib(void);
Suite *ts_figures(void);
Suite *ts_fsm(void);

#endif
