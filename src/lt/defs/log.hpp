#pragma once

namespace lt {

#if defined(__GNUC__)
#define lt_log_fmt __attribute__((format(printf, 1, 2)))
#else
#define lt_log_fmt
#endif

#if defined(__GNUC__) || defined(__clang__)
#define gcc_fmt_diagnostic_push _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wformat-invalid-specifier\"") _Pragma("GCC diagnostic ignored \"-Wformat-extra-args\"")
#define gcc_fmt_diagnostic_pop _Pragma("GCC diagnostic pop")
#else
#define gcc_fmt_diagnostic_push
#define gcc_fmt_diagnostic_pop
#endif

lt_log_fmt void log(const char *data, ...);
lt_log_fmt void log_error(const char *data, ...);
void print(const char *data);
void print_error(const char *data);

#undef lt_log_fmt

} // namespace lt
