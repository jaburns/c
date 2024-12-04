#include "inc.h"

#ifdef __APPLE__

global mach_timebase_info_data_t g_timing_timebase;

internal void timing_global_init(void) {
    mach_timebase_info(&g_timing_timebase);
}

internal u64 timing_get_ticks(void) {
#if 0  // def __aarch64__
    u64 ret;
    __asm__ volatile("mrs %0, cntvct_el0" : "=r"(ret));
    return ret;
#else
    return mach_absolute_time();
#endif
}

internal u64 timing_ticks_to_nanos(u64 ticks) {
    return ticks * g_timing_timebase.numer / g_timing_timebase.denom;
}

#else

global u64 g_timing_start_secs;

internal void timing_global_init(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    g_timing_start_secs = ts.tv_sec;
}

internal u64 timing_get_ticks(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return 1000000000l * (ts.tv_sec - g_timing_start_secs) + ts.tv_nsec;
}

internal u64 timing_ticks_to_nanos(u64 ticks) {
    return ticks;
}

#endif