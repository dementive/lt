#pragma once

// #define STB_SPRINTF_NOFLOAT
#include "stb_sprintf.h"

namespace lt {

#ifndef LT_LOG_BUFFER_SIZE
#define LT_LOG_BUFFER_SIZE 1024
#endif

#if defined(__linux__) and (__GNUC__) and (__x86_64__)
#define LT_LOG_LINUX_X86(m_fd)                                                                                                                                                               \
	int result{};                                                                                                                                                                            \
	asm volatile("syscall" : "=a"(result) : "0"(1), "D"(m_fd), "S"(buf), "d"(len) : "rcx", "r11", "memory");
#else
#include <stdio.h>
#endif

#define lt_log_fmt()                                                                                                                                                                         \
	char buf[LT_LOG_BUFFER_SIZE];                                                                                                                                                            \
	va_list va;                                                                                                                                                                              \
	va_start(va, data);                                                                                                                                                                      \
	int len = stb::vsprintf(buf, data, va);                                                                                                                                                  \
	buf[len] = '\n';                                                                                                                                                                         \
	buf[len + 1] = '\0';                                                                                                                                                                     \
	len += 1;                                                                                                                                                                                \
	va_end(va)

inline void log(const char *data, ...) {
	lt_log_fmt();

#ifdef LT_LOG_LINUX_X86
	LT_LOG_LINUX_X86(1);
#else
	fputs(buf, stdout);
#endif
}

inline void print(const char *data) { lt::log("%s", data); }

inline void log_error(const char *data, ...) {
	lt_log_fmt();

#ifdef LT_LOG_LINUX_X86
	LT_LOG_LINUX_X86(2);
#else
	fputs(buf, stderr);
#endif
}

inline void print_error(const char *data) { lt::log_error("%s", data); }

#undef LT_LOG_LINUX_X86
#undef LT_LOG_BUFFER_SIZE
#undef lt_log_fmt

} // namespace lt