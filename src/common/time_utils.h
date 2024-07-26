#ifndef TIME_UTILS
#define TIME_UTILS

#define _POSIX_C_SOURCE 200809L
// relying on the POSIX clock_gettime()
// https://pubs.opengroup.org/onlinepubs/9699919799/functions/clock_getres.html
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

unsigned long get_timespec_diff_ms(const struct timespec *later,
                                   const struct timespec *earlier);
bool get_is_time_to_operate_ms_diff(struct timespec *prev_op,
                                    const unsigned long ms_diff_threshold);

#endif