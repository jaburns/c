#include "inc.h"

#ifdef __APPLE__
#ifdef __aarch64__

global mach_timebase_info_data_t g_timing_timebase;

internal void timing_global_init(void) {
    mach_timebase_info(&g_timing_timebase);
}

internal u64 timing_get_ticks(void) {
    u64 ret;
    __asm__ volatile("mrs %0, cntvct_el0" : "=r"(ret));
    return ret;
}

internal u64 timing_ticks_to_nanos(u64 ticks) {
    return ticks * g_timing_timebase.numer / g_timing_timebase.denom;
}

#else

global mach_timebase_info_data_t g_timing_timebase;
global u64                       g_timing_start_ticks;

internal void timing_global_init(void) {
    mach_timebase_info(&g_timing_timebase);
    g_timing_start_ticks = mach_absolute_time();
}

internal u64 timing_get_nanos_since_start(void) {
    u64 elapsed    = mach_absolute_time() - g_timing_start_ticks;
    u64 elapsed_ns = elapsed * g_timing_timebase.numer / g_timing_timebase.denom;
    return elapsed_ns;
}

#endif
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