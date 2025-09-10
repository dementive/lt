#pragma once

namespace lt {

#if defined(__GNUC__)
#define lt_log_fmt __attribute__((format(printf, 1, 2)))
#else
#define lt_log_fmt
#endif

lt_log_fmt void log(const char *data, ...);
lt_log_fmt void log_error(const char *data, ...);
void print(const char *data);
void print_error(const char *data);

#undef lt_log_fmt

} // namespace lt
