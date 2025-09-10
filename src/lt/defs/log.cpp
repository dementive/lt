#include "log.hpp"

#include "stb_sprintf.h"

#include <cstring>

#ifndef lt_log_buffer_size
#define lt_log_buffer_size 1024
#endif

#if defined(__linux__) and (__GNUC__) and (__x86_64__)
#define lt_log_linux_x86(m_fd)                                                                                                                                                               \
	int result{};                                                                                                                                                                            \
	asm volatile("syscall" : "=a"(result) : "0"(1), "D"(m_fd), "S"(buf), "d"(len) : "rcx", "r11", "memory");
#else
#include <stdio.h>
#endif

#define lt_log_vsprintf()                                                                                                                                                                    \
	char buf[lt_log_buffer_size];                                                                                                                                                            \
	va_list va;                                                                                                                                                                              \
	va_start(va, data);                                                                                                                                                                      \
	int len = stb::vsprintf(buf, data, va);                                                                                                                                                  \
	buf[len] = '\n';                                                                                                                                                                         \
	buf[len + 1] = '\0';                                                                                                                                                                     \
	len += 1;                                                                                                                                                                                \
	va_end(va)

void lt::log(const char *data, ...) {
	lt_log_vsprintf();

#ifdef lt_log_linux_x86
	lt_log_linux_x86(1);
#else
	fputs(buf, stdout);
#endif
}

void lt::print(const char *data) { lt::log("%s", data); }

void lt::log_error(const char *data, ...) {
	lt_log_vsprintf();

#ifdef lt_log_linux_x86
	lt_log_linux_x86(2);
#else
	fputs(buf, stderr);
#endif
}

void lt::print_error(const char *data) { lt::log_error("%s", data); }
