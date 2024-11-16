#include "inc.h"

global u64 g_timer_start_secs;

internal void test_timer_init(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    timer_start_secs = ts.tv_sec;
}

internal u64 test_timer_get_nanos(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return 1000000000l * (ts.tv_sec - timer_start_secs) + ts.tv_nsec;
}

internal void test_run_(char* name, void (*fn)(void)) {
    test_timer_init();
    fprintf(stderr, "> Running: %s", name);
    u64 start_nanos = test_timer_get_nanos();
    fn();
    u64 finish_nanos = test_timer_get_nanos();
    fprintf(stderr, "\t- completed in %llu μs\n", (finish_nanos - start_nanos) / 1000);
}

internal void test_base(void) {
    test_run(test_channel);
}