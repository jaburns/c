#include "inc.h"

#ifdef __APPLE__

global mach_timebase_info_data_t g_timing_timebase;
global u64 g_timing_start_ticks;

internal void timing_global_init(void) {
    mach_timebase_info(&g_timing_timebase);
    g_timing_start_ticks = mach_absolute_time();
}

internal u64 timing_get_nanos_since_start(void) {
    u64 elapsed = mach_absolute_time() - g_timing_start_ticks;
    u64 elapsed_ns = elapsed * g_timing_timebase.numer / g_timing_timebase.denom;
    return elapsed_ns;
}

#else

global u64 g_timing_start_secs;

internal void timing_global_init(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    g_timing_start_secs = ts.tv_sec;
}

internal u64 timing_get_nanos_since_start(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return 1000000000l * (ts.tv_sec - g_timing_start_secs) + ts.tv_nsec;
}

#endif