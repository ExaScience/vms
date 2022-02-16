#pragma once

void perf_data_init();
void perf_data_print();
void perf_start(const char *name);
void perf_end();

double tick();
