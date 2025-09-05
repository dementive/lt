#include "perf.hpp"

#if defined(__linux__)
#include <linux/perf_event.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>

#include "lt/fixed_string.hpp"
#include "lt/fixed_vector.hpp"

#define SC_PERF_HW_CACHE(CACHE, OP, RESULT) ((PERF_COUNT_HW_CACHE_##CACHE) | (PERF_COUNT_HW_CACHE_OP_##OP << 8u) | (PERF_COUNT_HW_CACHE_RESUsc_##RESULT << 16u))
#define SC_PERF_BUFFER_SIZE 100

struct sc_perf_event {
	const char *name;
	uint64_t type;
	uint64_t config;
};

struct sc_perf_item {
	struct sc_perf_event event;
	double value;
	int fd;
};

// clang-format off
static const struct sc_perf_event sc_perf_hw[] = {
        // {"cpu-clock",               PERF_TYPE_SOFTWARE, PERF_COUNT_SW_CPU_CLOCK                  },
        // {"task-clock",              PERF_TYPE_SOFTWARE, PERF_COUNT_SW_TASK_CLOCK                 },
        // {"page-faults",             PERF_TYPE_SOFTWARE, PERF_COUNT_SW_PAGE_FAULTS                },
        // {"context-switches",        PERF_TYPE_SOFTWARE, PERF_COUNT_SW_CONTEXT_SWITCHES           },
        // {"cpu-migrations",          PERF_TYPE_SOFTWARE, PERF_COUNT_SW_CPU_MIGRATIONS             },
        // {"page-fault-minor",        PERF_TYPE_SOFTWARE, PERF_COUNT_SW_PAGE_FAULTS_MIN            },
     // {"page-fault-major",        PERF_TYPE_SOFTWARE, PERF_COUNT_SW_PAGE_FAULTS_MAJ            },
     // {"alignment-faults",        PERF_TYPE_SOFTWARE, PERF_COUNT_SW_ALIGNMENT_FAULTS           },
     // {"emulation-faults",        PERF_TYPE_SOFTWARE, PERF_COUNT_SW_EMULATION_FAULTS           },

        {"cpu-cycles",              PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES                 },
        {"instructions",            PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS               },
     // {"cache-references",        PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_REFERENCES           },
        {"cache-misses",            PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_MISSES               },
     // {"branch-instructions",     PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_INSTRUCTIONS        },
     {"branch-misses",           PERF_TYPE_HARDWARE, PERF_COUNT_HW_BRANCH_MISSES              },
     // {"bus-cycles",              PERF_TYPE_HARDWARE, PERF_COUNT_HW_BUS_CYCLES                 },
     // {"stalled-cycles-frontend", PERF_TYPE_HARDWARE, PERF_COUNT_HW_STALLED_CYCLES_FRONTEND    },
     // {"stalled-cycles-backend",  PERF_TYPE_HARDWARE, PERF_COUNT_HW_STALLED_CYCLES_BACKEND     },
     // {"ref-cpu-cycles",          PERF_TYPE_HARDWARE, PERF_COUNT_HW_REF_CPU_CYCLES             },

     // {"L1D-read-access",         PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(L1D, READ, ACCESS)      },
        // {"L1D-read-miss",           PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(L1D, READ, MISS)        },
     // {"L1D-write-access",        PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(L1D, WRITE, ACCESS)     },
     // {"L1D-write-miss",          PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(L1D, WRITE, MISS)       },
     // {"L1D-prefetch-access",     PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(L1D, PREFETCH, ACCESS)  },
     // {"L1D-prefetch-miss",       PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(L1D, PREFETCH, MISS)    },
     // {"L1I-read-access",         PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(L1I, READ, ACCESS)      },
        // {"L1I-read-miss",           PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(L1I, READ, MISS)        },
     // {"L1I-write-access",        PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(L1I, WRITE, ACCESS)     },
     // {"L1I-write-miss",          PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(L1I, WRITE, MISS)       },
     // {"L1I-prefetch-access",     PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(L1I, PREFETCH, ACCESS)  },
     // {"L1I-prefetch-miss",       PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(L1I, PREFETCH, MISS)    },
     // {"LL-read-access",          PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(LL, READ, ACCESS)       },
     // {"LL-read-miss",            PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(LL, READ, MISS)         },
     // {"LL-write-access",         PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(LL, WRITE, ACCESS)      },
     // {"LL-write-miss",           PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(LL, WRITE, MISS)        },
     // {"LL-prefetch-access",      PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(LL, PREFETCH, ACCESS)   },
     // {"LL-prefetch-miss",        PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(LL, PREFETCH, MISS)     },
     // {"DTLB-read-access",        PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(DTLB, READ, ACCESS)     },
     // {"DTLB-read-miss",          PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(DTLB, READ, MISS)       },
     // {"DTLB-write-access",       PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(DTLB, WRITE, ACCESS)    },
     // {"DTLB-write-miss",         PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(DTLB, WRITE, MISS)      },
     // {"DTLB-prefetch-access",    PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(DTLB, PREFETCH, ACCESS) },
     // {"DTLB-prefetch-miss",      PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(DTLB, PREFETCH, MISS)   },
     // {"ITLB-read-access",        PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(ITLB, READ, ACCESS)     },
     // {"ITLB-read-miss",          PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(ITLB, READ, MISS)       },
     // {"ITLB-write-access",       PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(ITLB, WRITE, ACCESS)    },
     // {"ITLB-write-miss",         PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(ITLB, WRITE, MISS)      },
     // {"ITLB-prefetch-access",    PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(ITLB, PREFETCH, ACCESS) },
     // {"ITLB-prefetch-miss",      PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(ITLB, PREFETCH, MISS)   },
     // {"BPU-read-access",         PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(BPU, READ, ACCESS)      },
     // {"BPU-read-miss",           PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(BPU, READ, MISS)        },
     // {"BPU-write-access",        PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(BPU, WRITE, ACCESS)     },
     // {"BPU-write-miss",          PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(BPU, WRITE, MISS)       },
     // {"BPU-prefetch-access",     PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(BPU, PREFETCH, ACCESS)  },
     // {"BPU-prefetch-miss",       PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(BPU, PREFETCH, MISS)    },
     // {"NODE-read-access",        PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(NODE, READ, ACCESS)     },
     // {"NODE-read-miss",          PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(NODE, READ, MISS)       },
     // {"NODE-write-access",       PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(NODE, WRITE, ACCESS)    },
     // {"NODE-write-miss",         PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(NODE, WRITE, MISS)      },
     // {"NODE-prefetch-access",    PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(NODE, PREFETCH, ACCESS) },
     // {"NODE-prefetch-miss",      PERF_TYPE_HW_CACHE, SC_PERF_HW_CACHE(NODE, PREFETCH, MISS)   },
};

// clang-format on
#define ITEMS_SIZE (sizeof(sc_perf_hw) / sizeof(struct sc_perf_event))

namespace {

int init = 0;
int running = 0;
uint64_t total = 0;
uint64_t start = 0;

struct sc_perf_item sc_perf_items[ITEMS_SIZE];

#define sc_perf_assert(val)                                                                                                                                                                  \
	do {                                                                                                                                                                                     \
		if (!(val)) {                                                                                                                                                                        \
			fprintf(stderr, "%s:%d: error", __FILE__, __LINE__);                                                                                                                             \
			abort();                                                                                                                                                                         \
		}                                                                                                                                                                                    \
	} while (0)

void sc_perf_set(struct sc_perf_item *items, size_t size) {
	const uint64_t flags = PERF_FORMAT_TOTAL_TIME_ENABLED | PERF_FORMAT_TOTAL_TIME_RUNNING;
	int fd;

	for (size_t i = 0; i < size; i++) {
		struct perf_event_attr p = {
			.type = static_cast<__u32>(items[i].event.type),
			.size = sizeof(struct perf_event_attr),
			.config = items[i].event.config,
			.read_format = flags,
			.disabled = 1,
			.inherit = 1,
			.inherit_stat = 0,
		};
		p.exclude_kernel = 0;
		p.exclude_hv = 0;

		fd = (int)syscall(__NR_perf_event_open, &p, 0, -1, -1, PERF_FLAG_FD_CLOEXEC);
		if (fd == -1) {
			fprintf(stderr, "Failed to set counter : %s , try running: 'sudo sysctl -w kernel.perf_event_paranoid=-1'\n", items[i].event.name);
			abort();
		}

		items[i].fd = fd;
	}
}

void sc_read(struct sc_perf_item *items, size_t size) {
	struct read_format {
		uint64_t value;
		uint64_t time_enabled;
		uint64_t time_running;
	} fmt;

	for (size_t i = 0; i < size; i++) {
		double n = 1.0;

		sc_perf_assert(read(items[i].fd, &fmt, sizeof(fmt)) == sizeof(fmt));

		if (fmt.time_enabled > 0 && fmt.time_running > 0)
			n = (double)fmt.time_running / (double)fmt.time_enabled;

		items[i].value += (double)fmt.value * n;
	}
}

void sc_perf_clear() {
	total = 0;
	start = 0;
	running = 0;
	init = 0;

	for (size_t i = 0; i < ITEMS_SIZE; i++) {
		sc_perf_items[i].event = sc_perf_hw[i];
		sc_perf_items[i].value = 0;
		sc_perf_items[i].fd = -1;
	}
}

uint64_t sy_time_nano() {
	int rc;
	struct timespec ts;

	rc = clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	if (rc == -1)
		abort();

	return ((uint64_t)(ts.tv_nsec + (ts.tv_sec * 1000 * 1000 * 1000)));
}

lt::fixed_vector<perf_stats, SC_PERF_BUFFER_SIZE> stored_perf_stats;

void print_perf_stat(const perf_stats &stat) {
	printf("\n| %-25s | %-18s \n", stat.name, "Value");
	printf("-----------------------------------------\n");
	printf("| %-25s | %.2f ms\n", "time", stat.time);

	printf("| %-25s | %lu \n", "cpu_cycles", stat.cpu_cycles);
	printf("| %-25s | %lu \n", "instructions", stat.instructions);
	printf("| %-25s | %lu \n", "cache_misses", stat.cache_misses);
	printf("| %-25s | %lu \n", "branch_misses", stat.branch_misses);
}

} // namespace

lt::span<perf_stats> get_perf_stats() { return { stored_perf_stats.data, static_cast<size_t>(stored_perf_stats.size()) }; }

void print_perf_stats() {
	for (const perf_stats &stat : stored_perf_stats)
		print_perf_stat(stat);
}

void perf_start() {
	if (!init) {
		sc_perf_clear();
		sc_perf_set(sc_perf_items, ITEMS_SIZE);
		init = 1;
	}

	sc_perf_assert(prctl(PR_TASK_PERF_EVENTS_ENABLE) == 0);

	start = sy_time_nano();
	running = 1;
}

void perf_pause() {
	sc_perf_assert(init);

	if (!running)
		return;

	sc_perf_assert(prctl(PR_TASK_PERF_EVENTS_DISABLE) == 0);

	total += sy_time_nano() - start;
	running = 0;
}

void perf_end(const char *p_test_name, bool p_stdout) {
	sc_perf_assert(init);

	perf_pause();
	sc_read(sc_perf_items, ITEMS_SIZE);

	for (const sc_perf_item &item : sc_perf_items)
		close(item.fd);

	perf_stats stats;
	stats.name = p_test_name;
	stats.time = (double)total / 1e6;

	for (const sc_perf_item &item : sc_perf_items) {
		const double value = item.value;

		if (lt::cstr(item.event.name) == "cpu-cycles")
			stats.cpu_cycles = value;
		else if (lt::cstr(item.event.name) == "instructions")
			stats.instructions = value;
		else if (lt::cstr(item.event.name) == "cache-misses")
			stats.cache_misses = value;
		else if (lt::cstr(item.event.name) == "branch-misses")
			stats.branch_misses = value;
	}

	if (p_stdout)
		print_perf_stat(stats);

	stored_perf_stats.push_back(stats);
	sc_perf_clear();
}

#else

void perf_start() {}
void perf_pause() {}
void perf_end(const char *p_test_name) {}
lt::span<perf_stats> get_perf_stats() { return {}; }
void print_perf_stats() {}

#endif
