#pragma once

namespace sc {

namespace detail {

enum sc_log_level : unsigned char {
	SC_LOG_DEBUG,
	SC_LOG_INFO,
	SC_LOG_WARN,
	SC_LOG_ERROR,
	SC_LOG_OFF,
};

// Internal function
int sc_log_log(enum sc_log_level level, const char *fmt, ...);

} // namespace detail

// Max file size to rotate, should not be more than 4 GB.
#define SC_LOG_FILE_SIZE (2 * 1024 * 1024)

#ifdef DEBUG_ENABLED
#define sc_log_ap(fmt, ...) "(%s:%d) " fmt "\n", strrchr("/" __FILE__, '/') + 1, __LINE__, __VA_ARGS__
#else
#define sc_log_ap(fmt, ...) fmt "\n", __VA_ARGS__
#endif

/**
 * sc_log_init() call once in your application before using log functions.
 * sc_log_term() call once to clean up, you must not use logger after this call.
 * These functions are not thread-safe, should be called from a single thread.
 *
 * @return '0' on success, negative value on error, errno will be set.
 */
int sc_log_init();
int sc_log_term();

/**
 * Set thread name.
 *
 * Call once from each thread if you want to set thread name.
 * @param name  Thread name
 */
void sc_log_set_thread_name(const char *name);

/**
 * Set log level.
 *
 * @param level  One of "debug", "info", "warn", "error", "off"
 * @return       '0' on success, negative value on invalid level string
 */
int sc_log_set_level(const char *level);

/**
 * Enable stdout logging.
 *
 * @param enable 'true' to enable, 'false' to disable logging to stdout.
 */
void sc_log_set_stdout(bool enable);

/**
 * Enable file logging.
 *
 * Log files will be rotated to prevent generating very big files. Once current
 * log file reaches 'SC_LOG_FILE_SIZE' (see definition above), it will be
 * renamed as `prev` and the latest logs will always be in the 'current' file.
 *
 * e.g., sc_log_set_file("/tmp/log.0.txt", "/tmp/log-latest.txt");
 *
 * To disable logging into file: sc_log_set_file(NULL, NULL);
 *
 * @param prev     file path for previous log file, 'NULL' to disable
 * @param current  file path for latest log file, 'NULL' to disable
 * @return         0 on success, -1 on error, errno will be set.
 */
int sc_log_set_file(const char *prev, const char *current);

void log(const char *p_str);

#define log_debug(...) (sc::detail::sc_log_log(sc::detail::SC_LOG_DEBUG, sc_log_ap(__VA_ARGS__, "")))
#define log_info(...) (sc::detail::sc_log_log(sc::detail::SC_LOG_INFO, sc_log_ap(__VA_ARGS__, "")))
#define log_warn(...) (sc::detail::sc_log_log(sc::detail::SC_LOG_WARN, sc_log_ap(__VA_ARGS__, "")))
#define log_error(...) (sc::detail::sc_log_log(sc::detail::SC_LOG_ERROR, sc_log_ap(__VA_ARGS__, "")))

} // namespace sc
