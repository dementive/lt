#pragma once

#include "lt/span.hpp"
#include "lt/types/defer.hpp"

void perf_start();
void perf_pause();
void perf_end(const char *p_test_name, bool p_stdout = false);

struct perf_stats {
	const char *name;
	double time;
	unsigned long cpu_cycles;
	unsigned long instructions;
	unsigned long cache_misses;
	unsigned long branch_misses;
};

lt::span<perf_stats> get_perf_stats();
void print_perf_stats();

#define perf_scope(m_test_name)                                                                                                                                                              \
	perf_start();                                                                                                                                                                            \
	const char *perf_test_name_ = m_test_name;                                                                                                                                               \
	auto _ = lt::defer([&perf_test_name_] { perf_end(perf_test_name_); });

#define perf_func perf_scope(__FUNCTION__)
