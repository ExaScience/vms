#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

void perf_data_init();
void perf_data_print();
void perf_start(const char *name);
void perf_end(const char *name);
 
double tick();

#ifdef __cplusplus
}
#endif
