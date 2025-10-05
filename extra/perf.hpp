#pragma once

#include "lt/types/defer.hpp"

void perf_start();
void perf_pause();
void perf_end(const char *p_test_name);

struct perf_stats {
	const char *name;

	unsigned long task_clock;
	unsigned long cpu_cycles;
	unsigned long instructions;
	unsigned long cache_misses;
	unsigned long branch_misses;
};

#define perf_scope(m_test_name)                                                                                                                                                              \
	perf_start();                                                                                                                                                                            \
	auto _ = lt::defer([](const char *perf_test_name_ = m_test_name) { perf_end(perf_test_name_); });

#define perf_func perf_scope(__FUNCTION__)
