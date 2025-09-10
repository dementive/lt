#pragma once

// See https://github.com/nothings/stb/blob/master/stb_sprintf.h for full usage details
// Everything is the same except:
// - It is split into a h and cpp file.
// - All symbols are in the stb namespace
// - The header doens't leak any macros

#include <stdarg.h>

namespace stb {

#if defined(__GNUC__)
#define stbsp_attribute_format(fmt, va) __attribute__((format(printf, fmt, va)))
#else
#define stbsp_attribute_format(fmt, va)
#endif

int vsprintf(char *buf, char const *fmt, va_list va);
int vsnprintf(char *buf, int count, const char *fmt, va_list va);
int sprintf(char *buf, const char *fmt, ...) stbsp_attribute_format(2, 3);
int snprintf(char *buf, int count, const char *fmt, ...) stbsp_attribute_format(3, 4);

using stbsp_sprintfcb = char *(const char *, void *, int);
int vsprintfcb(stbsp_sprintfcb *callback, void *user, char *buf, const char *fmt, va_list va);

#undef stbsp_attribute_format

} // namespace stb
