#pragma once

#define STB_SPRINTF_IMPLEMENTATION
#define STB_SPRINTF_NOFLOAT
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

inline void log(const char *data, ...) {
	char buf[LT_LOG_BUFFER_SIZE];

	va_list va;
	va_start(va, data);
	int len = stbsp_vsprintf(buf, data, va);
	va_end(va);

#ifdef LT_LOG_LINUX_X86
	LT_LOG_LINUX_X86(1);
#else
	printf("%s", buf);
#endif
}

inline void log_error(const char *data, ...) {
	char buf[LT_LOG_BUFFER_SIZE];

	va_list va;
	va_start(va, data);
	int len = stbsp_vsprintf(buf, data, va);
	va_end(va);

#ifdef LT_LOG_LINUX_X86
	LT_LOG_LINUX_X86(2);
#else
	fprintf(stderr, "%s", buf);
#endif
}

#undef LT_LOG_LINUX_X86
#undef LT_LOG_BUFFER_SIZE

} // namespace lt