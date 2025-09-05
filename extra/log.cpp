#include "log.hpp"
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctype.h>
#include <errno.h>
#include <time.h>

using namespace sc;
using namespace sc::detail;

// clang-format off
#if __STDC_VERSION__ >= 201112 && !defined __STDC_NO_ATOMIC__
    #define SC_ATOMIC
    #include <stdatomic.h>

    #define sc_atomic _Atomic
    #define sc_atomic_store(var, val)                                          \
        (atomic_store_explicit(var, val, memory_order_relaxed))
    #define sc_atomic_load(var)                                                \
        (atomic_load_explicit(var, memory_order_relaxed))
#else
    #define sc_atomic
    #define sc_atomic_store(var, val) ((*(var)) = (val))
    #define sc_atomic_load(var)       (*(var))
#endif
// clang-format on

thread_local char sc_name[32] = "Thread";

namespace {
// clang-format off
	static const struct sc_log_level_pair
	{
	    sc_log_level id;
	    const char *str;
	} sc_log_levels[] = {
	        {.id=SC_LOG_DEBUG, .str="debug"},
	        {.id=SC_LOG_INFO,  .str="info" },
	        {.id=SC_LOG_WARN,  .str="warn" },
	        {.id=SC_LOG_ERROR, .str="error"},
	        {.id=SC_LOG_OFF,   .str="off"  },
	};
// clang-format on
} // namespace

#if defined(_WIN32) || defined(_WIN64)

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#define strcasecmp _stricmp
#define localtime_r(a, b) (localtime_s(b, a) == 0 ? b : NULL)
#include <windows.h>

struct sc_log_mutex {
	CRITICAL_SECTION mtx;
};

int sc_log_mutex_init(struct sc_log_mutex *mtx) {
	InitializeCriticalSection(&mtx->mtx);
	return 0;
}

int sc_log_mutex_term(struct sc_log_mutex *mtx) {
	DeleteCriticalSection(&mtx->mtx);
	return 0;
}

void sc_log_mutex_lock(struct sc_log_mutex *mtx) { EnterCriticalSection(&mtx->mtx); }

void sc_log_mutex_unlock(struct sc_log_mutex *mtx) { LeaveCriticalSection(&mtx->mtx); }

#else

#include <pthread.h>

struct sc_log_mutex {
	pthread_mutex_t mtx;
};

int sc_log_mutex_init(struct sc_log_mutex *mtx) {
	int rc;

	pthread_mutexattr_t attr;
	pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

	mtx->mtx = mut;

	rc = pthread_mutexattr_init(&attr);
	if (rc != 0)
		return rc;

	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
	rc = pthread_mutex_init(&mtx->mtx, &attr);
	pthread_mutexattr_destroy(&attr);

	return rc;
}

void sc_log_mutex_term(struct sc_log_mutex *mtx) { pthread_mutex_destroy(&mtx->mtx); }

void sc_log_mutex_lock(struct sc_log_mutex *mtx) { pthread_mutex_lock(&mtx->mtx); }

void sc_log_mutex_unlock(struct sc_log_mutex *mtx) { pthread_mutex_unlock(&mtx->mtx); }

#endif

struct sc_log {
	FILE *fp;
	char current_file[256];
	char prev_file[256];
	size_t file_size;

	struct sc_log_mutex mtx;
	sc_atomic sc_log_level level;
	bool init;
	bool to_stdout;

	void *arg;
};

struct sc_log sc_log;

int sc::sc_log_init(void) {
	int rc;

	sc_log = { .fp = nullptr };

	sc_atomic_store(&sc_log.level, SC_LOG_INFO);
	sc_log.to_stdout = true;

	rc = sc_log_mutex_init(&sc_log.mtx);
	if (rc != 0)
		errno = rc;

	sc_log.init = true;

	return rc;
}

int sc::sc_log_term() {
	int rc = 0;

	if (!sc_log.init)
		return -1;

	if (sc_log.fp) {
		rc = fclose(sc_log.fp);
		if (rc != 0)
			rc = -1;
	}

	sc_log_mutex_term(&sc_log.mtx);
	sc_log = { .fp = nullptr };

	return rc;
}

void sc::sc_log_set_thread_name(const char *name) { strncpy(sc_name, name, sizeof(sc_name) - 1); }

static int sc_strcasecmp(const char *a, const char *b) {
	int n;

	for (;; a++, b++) {
		if (*a == 0 && *b == 0)
			return 0;

		n = tolower(*a) - tolower(*b);
		if (n != 0)
			return n;
	}
}

int sc::sc_log_set_level(const char *level) {
	size_t count = sizeof(sc_log_levels) / sizeof(sc_log_levels[0]);

	for (size_t i = 0; i < count; i++) {
		if (sc_strcasecmp(level, sc_log_levels[i].str) == 0) {
#ifdef SC_ATOMIC
			sc_atomic_store(&sc_log.level, sc_log_levels[i].id);
#else
			sc_log_mutex_lock(&sc_log.mtx);
			sc_log.level = sc_log_levels[i].id;
			sc_log_mutex_unlock(&sc_log.mtx);
#endif
			return 0;
		}
	}

	return -1;
}

void sc::sc_log_set_stdout(bool enable) {
	sc_log_mutex_lock(&sc_log.mtx);
	sc_log.to_stdout = enable;
	sc_log_mutex_unlock(&sc_log.mtx);
}

int sc::sc_log_set_file(const char *prev, const char *current) {
	int rc = 0, saved_errno = 0;
	long size;
	FILE *fp = NULL;

	sc_log_mutex_lock(&sc_log.mtx);

	if (sc_log.fp != NULL) {
		fclose(sc_log.fp);
		sc_log.fp = NULL;
	}

	sc_log.prev_file[0] = '\0';
	sc_log.current_file[0] = '\0';

	if (prev == NULL || current == NULL)
		goto out;

	if (strlen(prev) >= sizeof(sc_log.prev_file) - 1 || strlen(current) >= sizeof(sc_log.current_file) - 1)
		goto error;

	memcpy(sc_log.prev_file, prev, strlen(prev) + 1);
	memcpy(sc_log.current_file, current, strlen(current) + 1);

	fp = fopen(sc_log.current_file, "a+");
	if (fp == NULL)
		goto error;

	if (fprintf(fp, "\n") < 0)
		goto error;

	size = ftell(fp);
	if (size < 0)
		goto error;

	sc_log.file_size = (size_t)size;
	sc_log.fp = fp;

	goto out;

error:
	rc = -1;
	saved_errno = errno;

	if (fp != NULL)
		fclose(fp);
out:
	sc_log_mutex_unlock(&sc_log.mtx);
	errno = saved_errno;

	return rc;
}

static int sc_log_print_header(FILE *fp, enum sc_log_level level) {
	int rc;
	time_t t;
	struct tm result, *tm;

	t = time(nullptr);
	tm = localtime_r(&t, &result);

	if (tm == nullptr)
		return -1;

	rc = fprintf(fp, "[%02d:%02d:%02d][%s][%s] ", tm->tm_hour, tm->tm_min, tm->tm_sec, sc_log_levels[level].str, sc_name);
	if (rc < 0)
		return -1;

	return 0;
}

static int sc_log_stdout(enum sc_log_level level, const char *fmt, va_list va) {
	int rc;
	FILE *dest = level == SC_LOG_ERROR ? stderr : stdout;

	rc = sc_log_print_header(dest, level);
	if (rc < 0)
		return -1;
	rc = vfprintf(dest, fmt, va);
	if (rc < 0)
		return -1;

	return 0;
}

static int sc_log_file(enum sc_log_level level, const char *fmt, va_list va) {
	int rc, size;

	rc = sc_log_print_header(sc_log.fp, level);
	if (rc < 0)
		return -1;

	size = vfprintf(sc_log.fp, fmt, va);
	if (size < 0)
		return -1;

	sc_log.file_size += size;

	if (sc_log.file_size > (size_t)SC_LOG_FILE_SIZE) {
		fclose(sc_log.fp);
		(void)rename(sc_log.current_file, sc_log.prev_file);

		sc_log.fp = fopen(sc_log.current_file, "w+");
		if (sc_log.fp == nullptr)
			return -1;

		sc_log.file_size = 0;
	}

	return rc;
}

int sc::detail::sc_log_log(enum sc_log_level level, const char *fmt, ...) {
	int rc = 0;
	va_list va;

	// Use relaxed atomics to avoid locking cost, e.g., DEBUG logs when
	// level=INFO will get away without any synchronization on most
	// platforms.
#ifdef SC_ATOMIC
	enum sc_log_level curr;

	curr = sc_atomic_load(&sc_log.level);
	if (level < curr)
		return 0;
#endif

	sc_log_mutex_lock(&sc_log.mtx);

#ifndef SC_ATOMIC
	if (level < sc_log.level) {
		sc_log_mutex_unlock(&sc_log.mtx);
		return 0;
	}
#endif

	if (sc_log.to_stdout) {
		va_start(va, fmt);
		rc |= sc_log_stdout(level, fmt, va);
		va_end(va);
	}

	if (sc_log.fp != nullptr) {
		va_start(va, fmt);
		rc |= sc_log_file(level, fmt, va);
		va_end(va);
	}

	sc_log_mutex_unlock(&sc_log.mtx);

	return rc;
}

void sc::log(const char *p_str) { sc::detail::sc_log_log(sc::detail::SC_LOG_DEBUG, p_str); }
