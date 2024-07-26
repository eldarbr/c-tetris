#include "time_utils.h"

unsigned long get_timespec_diff_ms(const struct timespec *later,
                                   const struct timespec *earlier) {
  const unsigned long later_ms =
      later->tv_sec * 1000 + later->tv_nsec / 1000000;
  const unsigned long earlier_ms =
      earlier->tv_sec * 1000 + earlier->tv_nsec / 1000000;
  return later_ms - earlier_ms;
}

bool get_is_time_to_operate_ms_diff(struct timespec *prev_op,
                                    const unsigned long ms_diff_threshold) {
  struct timespec current_time = {0};
  clock_gettime(CLOCK_MONOTONIC, &current_time);
  const unsigned long ms_diff = get_timespec_diff_ms(&current_time, prev_op);
  bool should_operate = false;
  if (ms_diff >= ms_diff_threshold) {
    should_operate = true;
    prev_op->tv_nsec = current_time.tv_nsec;
    prev_op->tv_sec = current_time.tv_sec;
  }
  return should_operate;
}
