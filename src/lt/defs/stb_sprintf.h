#pragma once

// See https://github.com/nothings/stb/blob/master/stb_sprintf.h for full usage details

#include <stdarg.h>

namespace stb {

#if defined(__has_attribute)
#if __has_attribute(format)
#define STBSP_ATTRIBUTE_FORMAT(fmt, va) __attribute__((format(printf, fmt, va)))
#endif
#endif

#ifndef STBSP_ATTRIBUTE_FORMAT
#define STBSP_ATTRIBUTE_FORMAT(fmt, va)
#endif

int vsprintf(char *buf, char const *fmt, va_list va);
int vsnprintf(char *buf, int count, const char *fmt, va_list va);
int sprintf(char *buf, const char *fmt, ...) STBSP_ATTRIBUTE_FORMAT(2, 3);
int snprintf(char *buf, int count, const char *fmt, ...) STBSP_ATTRIBUTE_FORMAT(3, 4);

using stbsp_sprintfcb = char *(const char *, void *, int);
int vsprintfcb(stbsp_sprintfcb *callback, void *user, char *buf, const char *fmt, va_list va);

#undef STBSP_ATTRIBUTE_FORMAT

} // namespace stb
