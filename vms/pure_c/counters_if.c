#include <time.h>


double tick() {
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    return  (double)(t.tv_sec) + (double)(t.tv_nsec) / 1e9;
}

#ifndef VMS_PROFILING

void perf_data_init() {}
void perf_data_print() {}
void perf_start(const char *n) {}
void perf_end(const char *n) {}

#endif // VMS_PROFILING

