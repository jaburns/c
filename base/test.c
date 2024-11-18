#include "inc.h"

internal void test_run_(char* name, void (*fn)(void)) {
    timing_global_init();
    fprintf(stderr, "> Running: %s", name);
    u64 start_nanos = timing_get_nanos_since_start();
    fn();
    u64 finish_nanos = timing_get_nanos_since_start();
    fprintf(stderr, "\t- completed in %llu μs\n", (finish_nanos - start_nanos) / 1000);
}

#if TEST
internal void test_base(void) {
    test_run(test_channel);
}
#endif