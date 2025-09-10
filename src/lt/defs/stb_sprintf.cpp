#include "stb_sprintf.h"

#include <stddef.h> // size_t, ptrdiff_t

#define stbsp_uint32 unsigned int
#define stbsp_int32 signed int

#ifdef _MSC_VER
#define stbsp_uint64 unsigned __int64
#define stbsp_int64 signed __int64
#else
#define stbsp_uint64 unsigned long long
#define stbsp_int64 signed long long
#endif
#define stbsp_uint16 unsigned short

#ifndef stbsp_uintptr
#if defined(__ppc64__) || defined(__powerpc64__) || defined(__aarch64__) || defined(_M_X64) || defined(__x86_64__) || defined(__x86_64) || defined(__s390x__)
#define stbsp_uintptr stbsp_uint64
#else
#define stbsp_uintptr stbsp_uint32
#endif
#endif

#ifndef STB_SPRINTF_MSVC_MODE // used for MSVC2013 and earlier (MSVC2015 matches GCC)
#if defined(_MSC_VER) && (_MSC_VER < 1900)
#define STB_SPRINTF_MSVC_MODE
#endif
#endif

#ifdef STB_SPRINTF_NOUNALIGNED // define this before inclusion to force stbsp_sprintf to always use aligned accesses
#define STBSP_UNALIGNED(code)
#else
#define STBSP_UNALIGNED(code) code
#endif

#ifndef STB_SPRINTF_NOFLOAT
// internal float utility functions
static stbsp_int32 stbsp_real_to_str(const char **start, stbsp_uint32 *len, char *out, stbsp_int32 *decimal_pos, double value, stbsp_uint32 frac_digits);
static stbsp_int32 stbsp_real_to_parts(stbsp_int64 *bits, stbsp_int32 *expo, double value);
#define STBSP_SPECIAL 0x7000
#endif

static struct {
	short temp; // force next field to be 2-byte aligned
	char pair[201];
} stbsp_digitpair = { .temp = 0,
	.pair = "00010203040506070809101112131415161718192021222324"
			"25262728293031323334353637383940414243444546474849"
			"50515253545556575859606162636465666768697071727374"
			"75767778798081828384858687888990919293949596979899" };

#define STBSP_LEFTJUST 1
#define STBSP_LEADINGPLUS 2
#define STBSP_LEADINGSPACE 4
#define STBSP_LEADING_0X 8
#define STBSP_LEADINGZERO 16
#define STBSP_INTMAX 32
#define STBSP_TRIPLET_COMMA 64
#define STBSP_NEGATIVE 128
#define STBSP_METRIC_SUFFIX 256
#define STBSP_HALFWIDTH 512
#define STBSP_METRIC_NOSPACE 1024
#define STBSP_METRIC_1024 2048
#define STBSP_METRIC_JEDEC 4096

#ifdef _MSC_VER
#define STBSP_NOTUSED(v) (void)(v)
#else
#define STBSP_NOTUSED(v) (void)sizeof(v)
#endif

#if defined(__clang__)
#if defined(__has_feature) && defined(__has_attribute)
#if __has_feature(address_sanitizer)
#if __has_attribute(__no_sanitize__)
#define STBSP_ASAN __attribute__((__no_sanitize__("address")))
#elif __has_attribute(__no_sanitize_address__)
#define STBSP_ASAN __attribute__((__no_sanitize_address__))
#elif __has_attribute(__no_address_safety_analysis__)
#define STBSP_ASAN __attribute__((__no_address_safety_analysis__))
#endif
#endif
#endif
#elif defined(__GNUC__) && (__GNUC__ >= 5 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8))
#if defined(__SANITIZE_ADDRESS__) && __SANITIZE_ADDRESS__
#define STBSP_ASAN __attribute__((__no_sanitize_address__))
#endif
#endif

#ifndef STBSP_ASAN
#define STBSP_ASAN
#endif

#define STBSP_PUBLICDEF STBSP_ASAN

#ifndef STB_SPRINTF_MIN
#define STB_SPRINTF_MIN 512 // how many characters per callback
#endif

static void stbsp_lead_sign(stbsp_uint32 fl, char *sign) {
	sign[0] = 0;
	if (fl & STBSP_NEGATIVE) {
		sign[0] = 1;
		sign[1] = '-';
	} else if (fl & STBSP_LEADINGSPACE) {
		sign[0] = 1;
		sign[1] = ' ';
	} else if (fl & STBSP_LEADINGPLUS) {
		sign[0] = 1;
		sign[1] = '+';
	}
}

static STBSP_ASAN stbsp_uint32 stbsp_strlen_limited(const char *s, stbsp_uint32 limit) {
	const char *sn = s;

	// get up to 4-byte alignment
	for (;;) {
		if (((stbsp_uintptr)sn & 3) == 0)
			break;

		if (!limit || *sn == 0)
			return (stbsp_uint32)(sn - s);

		++sn;
		--limit;
	}

	// scan over 4 bytes at a time to find terminating 0
	// this will intentionally scan up to 3 bytes past the end of buffers,
	// but becase it works 4B aligned, it will never cross page boundaries
	// (hence the STBSP_ASAN markup; the over-read here is intentional
	// and harmless)
	while (limit >= 4) {
		stbsp_uint32 v = *(stbsp_uint32 *)sn;
		// bit hack to find if there's a 0 byte in there
		if ((v - 0x01010101) & (~v) & 0x80808080UL)
			break;

		sn += 4;
		limit -= 4;
	}

	// handle the last few characters to find actual size
	while (limit && *sn) {
		++sn;
		--limit;
	}

	return (stbsp_uint32)(sn - s);
}

STBSP_PUBLICDEF int stb::vsprintfcb(stb::stbsp_sprintfcb *callback, void *user, char *buf, const char *fmt, va_list va) {
	static char hex[] = "0123456789abcdefxp";
	static char hexu[] = "0123456789ABCDEFXP";
	char *bf;
	const char *f;
	int tlen = 0;

	bf = buf;
	f = fmt;
	for (;;) {
		stbsp_int32 fw, pr, tz;
		stbsp_uint32 fl;

// macros for the callback buffer stuff
#define stbsp_chk_cb_bufL(bytes)                                                                                                                                                             \
	{                                                                                                                                                                                        \
		int len = (int)(bf - buf);                                                                                                                                                           \
		if ((len + (bytes)) >= STB_SPRINTF_MIN) {                                                                                                                                            \
			tlen += len;                                                                                                                                                                     \
			if (0 == (bf = buf = callback(buf, user, len)))                                                                                                                                  \
				goto done;                                                                                                                                                                   \
		}                                                                                                                                                                                    \
	}
#define stbsp_chk_cb_buf(bytes)                                                                                                                                                              \
	{                                                                                                                                                                                        \
		if (callback) {                                                                                                                                                                      \
			stbsp_chk_cb_bufL(bytes);                                                                                                                                                        \
		}                                                                                                                                                                                    \
	}
#define stbsp_flush_cb()                                                                                                                                                                     \
	{ stbsp_chk_cb_bufL(STB_SPRINTF_MIN - 1); } // flush if there is even one byte in the buffer
#define stbsp_cb_buf_clamp(cl, v)                                                                                                                                                            \
	cl = v;                                                                                                                                                                                  \
	if (callback) {                                                                                                                                                                          \
		int lg = STB_SPRINTF_MIN - (int)(bf - buf);                                                                                                                                          \
		if (cl > lg)                                                                                                                                                                         \
			cl = lg;                                                                                                                                                                         \
	}

		// fast copy everything up to the next % (or end of string)
		for (;;) {
			while (((stbsp_uintptr)f) & 3) {
			schk1:
				if (f[0] == '%')
					goto scandd;
			schk2:
				if (f[0] == 0)
					goto endfmt;
				stbsp_chk_cb_buf(1);
				*bf++ = f[0];
				++f;
			}
			for (;;) {
				// Check if the next 4 bytes contain %(0x25) or end of string.
				// Using the 'hasless' trick:
				// https://graphics.stanford.edu/~seander/bithacks.html#HasLessInWord
				stbsp_uint32 v, c;
				v = *(stbsp_uint32 *)f;
				c = (~v) & 0x80808080;
				if (((v ^ 0x25252525) - 0x01010101) & c)
					goto schk1;
				if ((v - 0x01010101) & c)
					goto schk2;
				if (callback)
					if ((STB_SPRINTF_MIN - (int)(bf - buf)) < 4)
						goto schk1;
#ifdef STB_SPRINTF_NOUNALIGNED
				if (((stbsp_uintptr)bf) & 3) {
					bf[0] = f[0];
					bf[1] = f[1];
					bf[2] = f[2];
					bf[3] = f[3];
				} else
#endif
				{
					*(stbsp_uint32 *)bf = v;
				}
				bf += 4;
				f += 4;
			}
		}
	scandd:

		++f;

		// ok, we have a percent, read the modifiers first
		fw = 0;
		pr = -1;
		fl = 0;
		tz = 0;

		// flags
		for (;;) {
			switch (f[0]) {
				// if we have left justify
				case '-':
					fl |= STBSP_LEFTJUST;
					++f;
					continue;
				// if we have leading plus
				case '+':
					fl |= STBSP_LEADINGPLUS;
					++f;
					continue;
				// if we have leading space
				case ' ':
					fl |= STBSP_LEADINGSPACE;
					++f;
					continue;
				// if we have leading 0x
				case '#':
					fl |= STBSP_LEADING_0X;
					++f;
					continue;
				// if we have thousand commas
				case '\'':
					fl |= STBSP_TRIPLET_COMMA;
					++f;
					continue;
				// if we have kilo marker (none->kilo->kibi->jedec)
				case '$':
					if (fl & STBSP_METRIC_SUFFIX)
						if (fl & STBSP_METRIC_1024)
							fl |= STBSP_METRIC_JEDEC;
						else
							fl |= STBSP_METRIC_1024;
					else
						fl |= STBSP_METRIC_SUFFIX;
					++f;
					continue;
				// if we don't want space between metric suffix and number
				case '_':
					fl |= STBSP_METRIC_NOSPACE;
					++f;
					continue;
				// if we have leading zero
				case '0':
					fl |= STBSP_LEADINGZERO;
					++f;
					goto flags_done;
				default: goto flags_done;
			}
		}
	flags_done:

		// get the field width
		if (f[0] == '*') {
			fw = va_arg(va, stbsp_uint32);
			++f;
		} else {
			while ((f[0] >= '0') && (f[0] <= '9')) {
				fw = fw * 10 + f[0] - '0';
				f++;
			}
		}
		// get the precision
		if (f[0] == '.') {
			++f;
			if (f[0] == '*') {
				pr = va_arg(va, stbsp_uint32);
				++f;
			} else {
				pr = 0;
				while ((f[0] >= '0') && (f[0] <= '9')) {
					pr = pr * 10 + f[0] - '0';
					f++;
				}
			}
		}

		// handle integer size overrides
		switch (f[0]) {
			// are we halfwidth?
			case 'h':
				fl |= STBSP_HALFWIDTH;
				++f;
				if (f[0] == 'h')
					++f; // QUARTERWIDTH
				break;
			// are we 64-bit (unix style)
			case 'l':
				fl |= ((sizeof(long) == 8) ? STBSP_INTMAX : 0);
				++f;
				if (f[0] == 'l') {
					fl |= STBSP_INTMAX;
					++f;
				}
				break;
			// are we 64-bit on intmax? (c99)
			case 'j':
				fl |= (sizeof(size_t) == 8) ? STBSP_INTMAX : 0;
				++f;
				break;
			// are we 64-bit on size_t or ptrdiff_t? (c99)
			case 'z':
				fl |= (sizeof(ptrdiff_t) == 8) ? STBSP_INTMAX : 0;
				++f;
				break;
			case 't':
				fl |= (sizeof(ptrdiff_t) == 8) ? STBSP_INTMAX : 0;
				++f;
				break;
			// are we 64-bit (msft style)
			case 'I':
				if ((f[1] == '6') && (f[2] == '4')) {
					fl |= STBSP_INTMAX;
					f += 3;
				} else if ((f[1] == '3') && (f[2] == '2')) {
					f += 3;
				} else {
					fl |= ((sizeof(void *) == 8) ? STBSP_INTMAX : 0);
					++f;
				}
				break;
			default: break;
		}

		// handle each replacement
		switch (f[0]) {
#define STBSP_NUMSZ 512 // big enough for e308 (with commas) or e-307
			char num[STBSP_NUMSZ];
			char lead[8];
			char tail[8];
			char *s;
			const char *h;
			stbsp_uint32 l, n, cs;
			stbsp_uint64 n64;
#ifndef STB_SPRINTF_NOFLOAT
			double fv;
#endif
			stbsp_int32 dp;
			const char *sn;

			case 's':
				// get the string
				s = va_arg(va, char *);
				if (s == 0)
					s = (char *)"null";
				// get the length, limited to desired precision
				// always limit to ~0u chars since our counts are 32b
				l = stbsp_strlen_limited(s, (pr >= 0) ? pr : ~0u);
				lead[0] = 0;
				tail[0] = 0;
				pr = 0;
				dp = 0;
				cs = 0;
				// copy the string in
				goto scopy;

			case 'c': // char
				// get the character
				s = num + STBSP_NUMSZ - 1;
				*s = (char)va_arg(va, int);
				l = 1;
				lead[0] = 0;
				tail[0] = 0;
				pr = 0;
				dp = 0;
				cs = 0;
				goto scopy;

			case 'n': // weird write-bytes specifier
			{
				int *d = va_arg(va, int *);
				*d = tlen + (int)(bf - buf);
			} break;

#ifdef STB_SPRINTF_NOFLOAT
			case 'A': // float
			case 'a': // hex float
			case 'G': // float
			case 'g': // float
			case 'E': // float
			case 'e': // float
			case 'f': // float
				va_arg(va, double); // eat it
				s = (char *)"No float";
				l = 8;
				lead[0] = 0;
				tail[0] = 0;
				pr = 0;
				cs = 0;
				STBSP_NOTUSED(dp);
				goto scopy;
#else
			case 'A': // hex float
			case 'a': // hex float
				h = (f[0] == 'A') ? hexu : hex;
				fv = va_arg(va, double);
				if (pr == -1)
					pr = 6; // default is 6
				// read the double into a string
				if (stbsp_real_to_parts((stbsp_int64 *)&n64, &dp, fv))
					fl |= STBSP_NEGATIVE;

				s = num + 64;

				stbsp_lead_sign(fl, lead);

				if (dp == -1023)
					dp = (n64) ? -1022 : 0;
				else
					n64 |= (((stbsp_uint64)1) << 52);
				n64 <<= (64 - 56);
				if (pr < 15)
					n64 += ((((stbsp_uint64)8) << 56) >> (pr * 4));
				// add leading chars

#ifdef STB_SPRINTF_MSVC_MODE
				*s++ = '0';
				*s++ = 'x';
#else
				lead[1 + lead[0]] = '0';
				lead[2 + lead[0]] = 'x';
				lead[0] += 2;
#endif
				*s++ = h[(n64 >> 60) & 15];
				n64 <<= 4;
				if (pr)
					*s++ = '.';
				sn = s;

				// print the bits
				n = pr;
				if (n > 13)
					n = 13;
				if (pr > (stbsp_int32)n)
					tz = pr - n;
				pr = 0;
				while (n--) {
					*s++ = h[(n64 >> 60) & 15];
					n64 <<= 4;
				}

				// print the expo
				tail[1] = h[17];
				if (dp < 0) {
					tail[2] = '-';
					dp = -dp;
				} else
					tail[2] = '+';
				n = (dp >= 1000) ? 6 : ((dp >= 100) ? 5 : ((dp >= 10) ? 4 : 3));
				tail[0] = (char)n;
				for (;;) {
					tail[n] = '0' + dp % 10;
					if (n <= 3)
						break;
					--n;
					dp /= 10;
				}

				dp = (int)(s - sn);
				l = (int)(s - (num + 64));
				s = num + 64;
				cs = 1 + (3 << 24);
				goto scopy;

			case 'G': // float
			case 'g': // float
				h = (f[0] == 'G') ? hexu : hex;
				fv = va_arg(va, double);
				if (pr == -1)
					pr = 6;
				else if (pr == 0)
					pr = 1; // default is 6
				// read the double into a string
				if (stbsp_real_to_str(&sn, &l, num, &dp, fv, (pr - 1) | 0x80000000))
					fl |= STBSP_NEGATIVE;

				// clamp the precision and delete extra zeros after clamp
				n = pr;
				if (l > (stbsp_uint32)pr)
					l = pr;
				while ((l > 1) && (pr) && (sn[l - 1] == '0')) {
					--pr;
					--l;
				}

				// should we use %e
				if ((dp <= -4) || (dp > (stbsp_int32)n)) {
					if (pr > (stbsp_int32)l)
						pr = l - 1;
					else if (pr)
						--pr; // when using %e, there is one digit before the decimal
					goto doexpfromg;
				}
				// this is the insane action to get the pr to match %g semantics for %f
				if (dp > 0)
					pr = (dp < (stbsp_int32)l) ? l - dp : 0;
				else
					pr = -dp + ((pr > (stbsp_int32)l) ? (stbsp_int32)l : pr);
				goto dofloatfromg;

			case 'E': // float
			case 'e': // float
				h = (f[0] == 'E') ? hexu : hex;
				fv = va_arg(va, double);
				if (pr == -1)
					pr = 6; // default is 6
				// read the double into a string
				if (stbsp_real_to_str(&sn, &l, num, &dp, fv, pr | 0x80000000))
					fl |= STBSP_NEGATIVE;
			doexpfromg:
				tail[0] = 0;
				stbsp_lead_sign(fl, lead);
				if (dp == STBSP_SPECIAL) {
					s = (char *)sn;
					cs = 0;
					pr = 0;
					goto scopy;
				}
				s = num + 64;
				// handle leading chars
				*s++ = sn[0];

				if (pr)
					*s++ = '.';

				// handle after decimal
				if ((l - 1) > (stbsp_uint32)pr)
					l = pr + 1;
				for (n = 1; n < l; n++)
					*s++ = sn[n];
				// trailing zeros
				tz = pr - (l - 1);
				pr = 0;
				// dump expo
				tail[1] = h[0xe];
				dp -= 1;
				if (dp < 0) {
					tail[2] = '-';
					dp = -dp;
				} else
					tail[2] = '+';
#ifdef STB_SPRINTF_MSVC_MODE
				n = 5;
#else
				n = (dp >= 100) ? 5 : 4;
#endif
				tail[0] = (char)n;
				for (;;) {
					tail[n] = '0' + dp % 10;
					if (n <= 3)
						break;
					--n;
					dp /= 10;
				}
				cs = 1 + (3 << 24); // how many tens
				goto flt_lead;

			case 'f': // float
				fv = va_arg(va, double);
			doafloat:
				// do kilos
				if (fl & STBSP_METRIC_SUFFIX) {
					double divisor;
					divisor = 1000.0f;
					if (fl & STBSP_METRIC_1024)
						divisor = 1024.0;
					while (fl < 0x4000000) {
						if ((fv < divisor) && (fv > -divisor))
							break;
						fv /= divisor;
						fl += 0x1000000;
					}
				}
				if (pr == -1)
					pr = 6; // default is 6
				// read the double into a string
				if (stbsp_real_to_str(&sn, &l, num, &dp, fv, pr))
					fl |= STBSP_NEGATIVE;
			dofloatfromg:
				tail[0] = 0;
				stbsp_lead_sign(fl, lead);
				if (dp == STBSP_SPECIAL) {
					s = (char *)sn;
					cs = 0;
					pr = 0;
					goto scopy;
				}
				s = num + 64;

				// handle the three decimal varieties
				if (dp <= 0) {
					stbsp_int32 i;
					// handle 0.000*000xxxx
					*s++ = '0';
					if (pr)
						*s++ = '.';
					n = -dp;
					if ((stbsp_int32)n > pr)
						n = pr;
					i = n;
					while (i) {
						if ((((stbsp_uintptr)s) & 3) == 0)
							break;
						*s++ = '0';
						--i;
					}
					while (i >= 4) {
						*(stbsp_uint32 *)s = 0x30303030;
						s += 4;
						i -= 4;
					}
					while (i) {
						*s++ = '0';
						--i;
					}
					if ((stbsp_int32)(l + n) > pr)
						l = pr - n;
					i = l;
					while (i) {
						*s++ = *sn++;
						--i;
					}
					tz = pr - (n + l);
					cs = 1 + (3 << 24); // how many tens did we write (for commas below)
				} else {
					cs = (fl & STBSP_TRIPLET_COMMA) ? ((600 - (stbsp_uint32)dp) % 3) : 0;
					if ((stbsp_uint32)dp >= l) {
						// handle xxxx000*000.0
						n = 0;
						for (;;) {
							if ((fl & STBSP_TRIPLET_COMMA) && (++cs == 4)) {
								cs = 0;
								*s++ = ',';
							} else {
								*s++ = sn[n];
								++n;
								if (n >= l)
									break;
							}
						}
						if (n < (stbsp_uint32)dp) {
							n = dp - n;
							if ((fl & STBSP_TRIPLET_COMMA) == 0) {
								while (n) {
									if ((((stbsp_uintptr)s) & 3) == 0)
										break;
									*s++ = '0';
									--n;
								}
								while (n >= 4) {
									*(stbsp_uint32 *)s = 0x30303030;
									s += 4;
									n -= 4;
								}
							}
							while (n) {
								if ((fl & STBSP_TRIPLET_COMMA) && (++cs == 4)) {
									cs = 0;
									*s++ = ',';
								} else {
									*s++ = '0';
									--n;
								}
							}
						}
						cs = (int)(s - (num + 64)) + (3 << 24); // cs is how many tens
						if (pr) {
							*s++ = '.';
							tz = pr;
						}
					} else {
						// handle xxxxx.xxxx000*000
						n = 0;
						for (;;) {
							if ((fl & STBSP_TRIPLET_COMMA) && (++cs == 4)) {
								cs = 0;
								*s++ = ',';
							} else {
								*s++ = sn[n];
								++n;
								if (n >= (stbsp_uint32)dp)
									break;
							}
						}
						cs = (int)(s - (num + 64)) + (3 << 24); // cs is how many tens
						if (pr)
							*s++ = '.';
						if ((l - dp) > (stbsp_uint32)pr)
							l = pr + dp;
						while (n < l) {
							*s++ = sn[n];
							++n;
						}
						tz = pr - (l - dp);
					}
				}
				pr = 0;

				// handle k,m,g,t
				if (fl & STBSP_METRIC_SUFFIX) {
					char idx;
					idx = 1;
					if (fl & STBSP_METRIC_NOSPACE)
						idx = 0;
					tail[0] = idx;
					tail[1] = ' ';
					{
						if (fl >> 24) { // SI kilo is 'k', JEDEC and SI kibits are 'K'.
							if (fl & STBSP_METRIC_1024)
								tail[idx + 1] = "_KMGT"[fl >> 24];
							else
								tail[idx + 1] = "_kMGT"[fl >> 24];
							idx++;
							// If printing kibits and not in jedec, add the 'i'.
							if (fl & STBSP_METRIC_1024 && !(fl & STBSP_METRIC_JEDEC)) {
								tail[idx + 1] = 'i';
								idx++;
							}
							tail[0] = idx;
						}
					}
				};

			flt_lead:
				// get the length that we copied
				l = (stbsp_uint32)(s - (num + 64));
				s = num + 64;
				goto scopy;
#endif

			case 'B': // upper binary
			case 'b': // lower binary
				h = (f[0] == 'B') ? hexu : hex;
				lead[0] = 0;
				if (fl & STBSP_LEADING_0X) {
					lead[0] = 2;
					lead[1] = '0';
					lead[2] = h[0xb];
				}
				l = (8 << 4) | (1 << 8);
				goto radixnum;

			case 'o': // octal
				h = hexu;
				lead[0] = 0;
				if (fl & STBSP_LEADING_0X) {
					lead[0] = 1;
					lead[1] = '0';
				}
				l = (3 << 4) | (3 << 8);
				goto radixnum;

			case 'p': // pointer
				fl |= (sizeof(void *) == 8) ? STBSP_INTMAX : 0;
				pr = sizeof(void *) * 2;
				fl &= ~STBSP_LEADINGZERO; // 'p' only prints the pointer with zeros
										  // fall through - to X

			case 'X': // upper hex
			case 'x': // lower hex
				h = (f[0] == 'X') ? hexu : hex;
				l = (4 << 4) | (4 << 8);
				lead[0] = 0;
				if (fl & STBSP_LEADING_0X) {
					lead[0] = 2;
					lead[1] = '0';
					lead[2] = h[16];
				}
			radixnum:
				// get the number
				if (fl & STBSP_INTMAX)
					n64 = va_arg(va, stbsp_uint64);
				else
					n64 = va_arg(va, stbsp_uint32);

				s = num + STBSP_NUMSZ;
				dp = 0;
				// clear tail, and clear leading if value is zero
				tail[0] = 0;
				if (n64 == 0) {
					lead[0] = 0;
					if (pr == 0) {
						l = 0;
						cs = 0;
						goto scopy;
					}
				}
				// convert to string
				for (;;) {
					*--s = h[n64 & ((1 << (l >> 8)) - 1)];
					n64 >>= (l >> 8);
					if (!((n64) || ((stbsp_int32)((num + STBSP_NUMSZ) - s) < pr)))
						break;
					if (fl & STBSP_TRIPLET_COMMA) {
						++l;
						if ((l & 15) == ((l >> 4) & 15)) {
							l &= ~15;
							*--s = ',';
						}
					}
				};
				// get the tens and the comma pos
				cs = (stbsp_uint32)((num + STBSP_NUMSZ) - s) + ((((l >> 4) & 15)) << 24);
				// get the length that we copied
				l = (stbsp_uint32)((num + STBSP_NUMSZ) - s);
				// copy it
				goto scopy;

			case 'u': // unsigned
			case 'i':
			case 'd': // integer
				// get the integer and abs it
				if (fl & STBSP_INTMAX) {
					stbsp_int64 i64 = va_arg(va, stbsp_int64);
					n64 = (stbsp_uint64)i64;
					if ((f[0] != 'u') && (i64 < 0)) {
						n64 = (stbsp_uint64)-i64;
						fl |= STBSP_NEGATIVE;
					}
				} else {
					stbsp_int32 i = va_arg(va, stbsp_int32);
					n64 = (stbsp_uint32)i;
					if ((f[0] != 'u') && (i < 0)) {
						n64 = (stbsp_uint32)-i;
						fl |= STBSP_NEGATIVE;
					}
				}

#ifndef STB_SPRINTF_NOFLOAT
				if (fl & STBSP_METRIC_SUFFIX) {
					if (n64 < 1024)
						pr = 0;
					else if (pr == -1)
						pr = 1;
					fv = (double)(stbsp_int64)n64;
					goto doafloat;
				}
#endif

				// convert to string
				s = num + STBSP_NUMSZ;
				l = 0;

				for (;;) {
					// do in 32-bit chunks (avoid lots of 64-bit divides even with constant denominators)
					char *o = s - 8;
					if (n64 >= 100000000) {
						n = (stbsp_uint32)(n64 % 100000000);
						n64 /= 100000000;
					} else {
						n = (stbsp_uint32)n64;
						n64 = 0;
					}
					if ((fl & STBSP_TRIPLET_COMMA) == 0) {
						do {
							s -= 2;
							*(stbsp_uint16 *)s = *(stbsp_uint16 *)&stbsp_digitpair.pair[(n % 100) * 2];
							n /= 100;
						} while (n);
					}
					while (n) {
						if ((fl & STBSP_TRIPLET_COMMA) && (l++ == 3)) {
							l = 0;
							*--s = ',';
							--o;
						} else {
							*--s = (char)(n % 10) + '0';
							n /= 10;
						}
					}
					if (n64 == 0) {
						if ((s[0] == '0') && (s != (num + STBSP_NUMSZ)))
							++s;
						break;
					}
					while (s != o)
						if ((fl & STBSP_TRIPLET_COMMA) && (l++ == 3)) {
							l = 0;
							*--s = ',';
							--o;
						} else {
							*--s = '0';
						}
				}

				tail[0] = 0;
				stbsp_lead_sign(fl, lead);

				// get the length that we copied
				l = (stbsp_uint32)((num + STBSP_NUMSZ) - s);
				if (l == 0) {
					*--s = '0';
					l = 1;
				}
				cs = l + (3 << 24);
				if (pr < 0)
					pr = 0;

			scopy:
				// get fw=leading/trailing space, pr=leading zeros
				if (pr < (stbsp_int32)l)
					pr = l;
				n = pr + lead[0] + tail[0] + tz;
				if (fw < (stbsp_int32)n)
					fw = n;
				fw -= n;
				pr -= l;

				// handle right justify and leading zeros
				if ((fl & STBSP_LEFTJUST) == 0) {
					if (fl & STBSP_LEADINGZERO) // if leading zeros, everything is in pr
					{
						pr = (fw > pr) ? fw : pr;
						fw = 0;
					} else {
						fl &= ~STBSP_TRIPLET_COMMA; // if no leading zeros, then no commas
					}
				}

				// copy the spaces and/or zeros
				if (fw + pr) {
					stbsp_int32 i;
					stbsp_uint32 c;

					// copy leading spaces (or when doing %8.4d stuff)
					if ((fl & STBSP_LEFTJUST) == 0)
						while (fw > 0) {
							stbsp_cb_buf_clamp(i, fw);
							fw -= i;
							while (i) {
								if ((((stbsp_uintptr)bf) & 3) == 0)
									break;
								*bf++ = ' ';
								--i;
							}
							while (i >= 4) {
								*(stbsp_uint32 *)bf = 0x20202020;
								bf += 4;
								i -= 4;
							}
							while (i) {
								*bf++ = ' ';
								--i;
							}
							stbsp_chk_cb_buf(1);
						}

					// copy leader
					sn = lead + 1;
					while (lead[0]) {
						stbsp_cb_buf_clamp(i, lead[0]);
						lead[0] -= (char)i;
						while (i) {
							*bf++ = *sn++;
							--i;
						}
						stbsp_chk_cb_buf(1);
					}

					// copy leading zeros
					c = cs >> 24;
					cs &= 0xffffff;
					cs = (fl & STBSP_TRIPLET_COMMA) ? ((stbsp_uint32)(c - ((pr + cs) % (c + 1)))) : 0;
					while (pr > 0) {
						stbsp_cb_buf_clamp(i, pr);
						pr -= i;
						if ((fl & STBSP_TRIPLET_COMMA) == 0) {
							while (i) {
								if ((((stbsp_uintptr)bf) & 3) == 0)
									break;
								*bf++ = '0';
								--i;
							}
							while (i >= 4) {
								*(stbsp_uint32 *)bf = 0x30303030;
								bf += 4;
								i -= 4;
							}
						}
						while (i) {
							if ((fl & STBSP_TRIPLET_COMMA) && (cs++ == c)) {
								cs = 0;
								*bf++ = ',';
							} else
								*bf++ = '0';
							--i;
						}
						stbsp_chk_cb_buf(1);
					}
				}

				// copy leader if there is still one
				sn = lead + 1;
				while (lead[0]) {
					stbsp_int32 i;
					stbsp_cb_buf_clamp(i, lead[0]);
					lead[0] -= (char)i;
					while (i) {
						*bf++ = *sn++;
						--i;
					}
					stbsp_chk_cb_buf(1);
				}

				// copy the string
				n = l;
				while (n) {
					stbsp_int32 i;
					stbsp_cb_buf_clamp(i, n);
					n -= i;
					STBSP_UNALIGNED(while (i >= 4) {
						*(volatile stbsp_uint32 *)bf = *(volatile stbsp_uint32 *)s;
						bf += 4;
						s += 4;
						i -= 4;
					})
					while (i) {
						*bf++ = *s++;
						--i;
					}
					stbsp_chk_cb_buf(1);
				}

				// copy trailing zeros
				while (tz) {
					stbsp_int32 i;
					stbsp_cb_buf_clamp(i, tz);
					tz -= i;
					while (i) {
						if ((((stbsp_uintptr)bf) & 3) == 0)
							break;
						*bf++ = '0';
						--i;
					}
					while (i >= 4) {
						*(stbsp_uint32 *)bf = 0x30303030;
						bf += 4;
						i -= 4;
					}
					while (i) {
						*bf++ = '0';
						--i;
					}
					stbsp_chk_cb_buf(1);
				}

				// copy tail if there is one
				sn = tail + 1;
				while (tail[0]) {
					stbsp_int32 i;
					stbsp_cb_buf_clamp(i, tail[0]);
					tail[0] -= (char)i;
					while (i) {
						*bf++ = *sn++;
						--i;
					}
					stbsp_chk_cb_buf(1);
				}

				// handle the left justify
				if (fl & STBSP_LEFTJUST)
					if (fw > 0) {
						while (fw) {
							stbsp_int32 i;
							stbsp_cb_buf_clamp(i, fw);
							fw -= i;
							while (i) {
								if ((((stbsp_uintptr)bf) & 3) == 0)
									break;
								*bf++ = ' ';
								--i;
							}
							while (i >= 4) {
								*(stbsp_uint32 *)bf = 0x20202020;
								bf += 4;
								i -= 4;
							}
							while (i--)
								*bf++ = ' ';
							stbsp_chk_cb_buf(1);
						}
					}
				break;

			default: // unknown, just copy code
				s = num + STBSP_NUMSZ - 1;
				*s = f[0];
				l = 1;
				fw = fl = 0;
				lead[0] = 0;
				tail[0] = 0;
				pr = 0;
				dp = 0;
				cs = 0;
				goto scopy;
		}
		++f;
	}
endfmt:

	if (!callback)
		*bf = 0;
	else
		stbsp_flush_cb();

done:
	return tlen + (int)(bf - buf);
}

// ============================================================================
//   wrapper functions

STBSP_PUBLICDEF int stb::sprintf(char *buf, const char *fmt, ...) {
	int result;
	va_list va;
	va_start(va, fmt);
	result = stb::vsprintfcb(0, 0, buf, fmt, va);
	va_end(va);
	return result;
}

using stbsp_context = struct stbsp_context {
	char *buf;
	int count;
	int length;
	char tmp[STB_SPRINTF_MIN];
};

static char *stbsp_clamp_callback(const char *buf, void *user, int len) {
	stbsp_context *c = (stbsp_context *)user;
	c->length += len;

	if (len > c->count)
		len = c->count;

	if (len) {
		if (buf != c->buf) {
			const char *s, *se;
			char *d;
			d = c->buf;
			s = buf;
			se = buf + len;
			do {
				*d++ = *s++;
			} while (s < se);
		}
		c->buf += len;
		c->count -= len;
	}

	if (c->count <= 0)
		return c->tmp;
	return (c->count >= STB_SPRINTF_MIN) ? c->buf : c->tmp; // go direct into buffer if you can
}

static char *stbsp_count_clamp_callback(const char *buf, void *user, int len) {
	stbsp_context *c = (stbsp_context *)user;
	(void)sizeof(buf);

	c->length += len;
	return c->tmp; // go direct into buffer if you can
}

STBSP_PUBLICDEF int stb::vsnprintf(char *buf, int count, const char *fmt, va_list va) {
	stbsp_context c;

	if ((count == 0) && !buf) {
		c.length = 0;

		stb::vsprintfcb(stbsp_count_clamp_callback, &c, c.tmp, fmt, va);
	} else {
		int l;

		c.buf = buf;
		c.count = count;
		c.length = 0;

		stb::vsprintfcb(stbsp_clamp_callback, &c, stbsp_clamp_callback(0, &c, 0), fmt, va);

		// zero-terminate
		l = (int)(c.buf - buf);
		if (l >= count) // should never be greater, only equal (or less) than count
			l = count - 1;
		buf[l] = 0;
	}

	return c.length;
}

STBSP_PUBLICDEF int stb::snprintf(char *buf, int count, const char *fmt, ...) {
	int result;
	va_list va;
	va_start(va, fmt);

	result = stb::vsnprintf(buf, count, fmt, va);
	va_end(va);

	return result;
}

STBSP_PUBLICDEF int stb::vsprintf(char *buf, const char *fmt, va_list va) { return stb::vsprintfcb(0, 0, buf, fmt, va); }

// =======================================================================
//   low level float utility functions

#ifndef STB_SPRINTF_NOFLOAT

// copies d to bits w/ strict aliasing (this compiles to nothing on /Ox)
#define STBSP_COPYFP(dest, src)                                                                                                                                                              \
	{                                                                                                                                                                                        \
		int cn;                                                                                                                                                                              \
		for (cn = 0; cn < 8; cn++)                                                                                                                                                           \
			((char *)&dest)[cn] = ((char *)&src)[cn];                                                                                                                                        \
	}

// get float info
static stbsp_int32 stbsp_real_to_parts(stbsp_int64 *bits, stbsp_int32 *expo, double value) {
	double d;
	stbsp_int64 b = 0;

	// load value and round at the frac_digits
	d = value;

	STBSP_COPYFP(b, d);

	*bits = b & ((((stbsp_uint64)1) << 52) - 1);
	*expo = (stbsp_int32)(((b >> 52) & 2047) - 1023);

	return (stbsp_int32)((stbsp_uint64)b >> 63);
}

static constexpr double stbsp_bot[23] = { 1e+000, 1e+001, 1e+002, 1e+003, 1e+004, 1e+005, 1e+006, 1e+007, 1e+008, 1e+009, 1e+010, 1e+011, 1e+012, 1e+013, 1e+014, 1e+015, 1e+016, 1e+017,
	1e+018, 1e+019, 1e+020, 1e+021, 1e+022 };
static constexpr double stbsp_negbot[22] = { 1e-001, 1e-002, 1e-003, 1e-004, 1e-005, 1e-006, 1e-007, 1e-008, 1e-009, 1e-010, 1e-011, 1e-012, 1e-013, 1e-014, 1e-015, 1e-016, 1e-017, 1e-018,
	1e-019, 1e-020, 1e-021, 1e-022 };
static constexpr double stbsp_negboterr[22] = { -5.551115123125783e-018, -2.0816681711721684e-019, -2.0816681711721686e-020, -4.7921736023859299e-021, -8.1803053914031305e-022,
	4.5251888174113741e-023, 4.5251888174113739e-024, -2.0922560830128471e-025, -6.2281591457779853e-026, -3.6432197315497743e-027, 6.0503030718060191e-028, 2.0113352370744385e-029,
	-3.0373745563400371e-030, 1.1806906454401013e-032, -7.7705399876661076e-032, 2.0902213275965398e-033, -7.1542424054621921e-034, -7.1542424054621926e-035, 2.4754073164739869e-036,
	5.4846728545790429e-037, 9.2462547772103625e-038, -4.8596774326570872e-039 };
static constexpr double stbsp_top[13] = { 1e+023, 1e+046, 1e+069, 1e+092, 1e+115, 1e+138, 1e+161, 1e+184, 1e+207, 1e+230, 1e+253, 1e+276, 1e+299 };
static constexpr double stbsp_negtop[13] = { 1e-023, 1e-046, 1e-069, 1e-092, 1e-115, 1e-138, 1e-161, 1e-184, 1e-207, 1e-230, 1e-253, 1e-276, 1e-299 };
static constexpr double stbsp_toperr[13] = { 8388608, 6.8601809640529717e+028, -7.253143638152921e+052, -4.3377296974619174e+075, -1.5559416129466825e+098, -3.2841562489204913e+121,
	-3.7745893248228135e+144, -1.7356668416969134e+167, -3.8893577551088374e+190, -9.9566444326005119e+213, 6.3641293062232429e+236, -5.2069140800249813e+259, -5.2504760255204387e+282 };
static constexpr double stbsp_negtoperr[13] = { 3.9565301985100693e-040, -2.299904345391321e-063, 3.6506201437945798e-086, 1.1875228833981544e-109, -5.0644902316928607e-132,
	-6.7156837247865426e-155, -2.812077463003139e-178, -5.7778912386589953e-201, 7.4997100559334532e-224, -4.6439668915134491e-247, -6.3691100762962136e-270, -9.436808465446358e-293,
	8.0970921678014997e-317 };

#if defined(_MSC_VER) && (_MSC_VER <= 1200)
static stbsp_uint64 constexpr stbsp_powten[20] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000, 10000000000, 100000000000, 1000000000000, 10000000000000,
	100000000000000, 1000000000000000, 10000000000000000, 100000000000000000, 1000000000000000000, 10000000000000000000U };
#define stbsp_tento19th ((stbsp_uint64)1000000000000000000)
#else
static stbsp_uint64 constexpr stbsp_powten[20] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000, 10000000000ULL, 100000000000ULL, 1000000000000ULL,
	10000000000000ULL, 100000000000000ULL, 1000000000000000ULL, 10000000000000000ULL, 100000000000000000ULL, 1000000000000000000ULL, 10000000000000000000ULL };
#define stbsp_tento19th (1000000000000000000ULL)
#endif

#define stbsp_ddmulthi(oh, ol, xh, yh)                                                                                                                                                       \
	{                                                                                                                                                                                        \
		double ahi = 0, alo, bhi = 0, blo;                                                                                                                                                   \
		stbsp_int64 bt;                                                                                                                                                                      \
		oh = xh * yh;                                                                                                                                                                        \
		STBSP_COPYFP(bt, xh);                                                                                                                                                                \
		bt &= ((~(stbsp_uint64)0) << 27);                                                                                                                                                    \
		STBSP_COPYFP(ahi, bt);                                                                                                                                                               \
		alo = xh - ahi;                                                                                                                                                                      \
		STBSP_COPYFP(bt, yh);                                                                                                                                                                \
		bt &= ((~(stbsp_uint64)0) << 27);                                                                                                                                                    \
		STBSP_COPYFP(bhi, bt);                                                                                                                                                               \
		blo = yh - bhi;                                                                                                                                                                      \
		ol = ((ahi * bhi - oh) + ahi * blo + alo * bhi) + alo * blo;                                                                                                                         \
	}

#define stbsp_ddtoS64(ob, xh, xl)                                                                                                                                                            \
	{                                                                                                                                                                                        \
		double ahi = 0, alo, vh, t;                                                                                                                                                          \
		ob = (stbsp_int64)xh;                                                                                                                                                                \
		vh = (double)ob;                                                                                                                                                                     \
		ahi = (xh - vh);                                                                                                                                                                     \
		t = (ahi - xh);                                                                                                                                                                      \
		alo = (xh - (ahi - t)) - (vh + t);                                                                                                                                                   \
		ob += (stbsp_int64)(ahi + alo + xl);                                                                                                                                                 \
	}

#define stbsp_ddrenorm(oh, ol)                                                                                                                                                               \
	{                                                                                                                                                                                        \
		double s;                                                                                                                                                                            \
		s = oh + ol;                                                                                                                                                                         \
		ol = ol - (s - oh);                                                                                                                                                                  \
		oh = s;                                                                                                                                                                              \
	}

#define stbsp_ddmultlo(oh, ol, xh, xl, yh, yl) ol = ol + (xh * yl + xl * yh);

#define stbsp_ddmultlos(oh, ol, xh, yl) ol = ol + (xh * yl);

static void stbsp_raise_to_power10(double *ohi, double *olo, double d, stbsp_int32 power) // power can be -323 to +350
{
	double ph, pl;
	if ((power >= 0) && (power <= 22)) {
		stbsp_ddmulthi(ph, pl, d, stbsp_bot[power]);
	} else {
		stbsp_int32 e, et, eb;
		double p2h, p2l;

		e = power;
		if (power < 0)
			e = -e;
		et = (e * 0x2c9) >> 14; /* %23 */
		if (et > 13)
			et = 13;
		eb = e - (et * 23);

		ph = d;
		pl = 0.0;
		if (power < 0) {
			if (eb) {
				--eb;
				stbsp_ddmulthi(ph, pl, d, stbsp_negbot[eb]);
				stbsp_ddmultlos(ph, pl, d, stbsp_negboterr[eb]);
			}
			if (et) {
				stbsp_ddrenorm(ph, pl);
				--et;
				stbsp_ddmulthi(p2h, p2l, ph, stbsp_negtop[et]);
				stbsp_ddmultlo(p2h, p2l, ph, pl, stbsp_negtop[et], stbsp_negtoperr[et]);
				ph = p2h;
				pl = p2l;
			}
		} else {
			if (eb) {
				e = eb;
				if (eb > 22)
					eb = 22;
				e -= eb;
				stbsp_ddmulthi(ph, pl, d, stbsp_bot[eb]);
				if (e) {
					stbsp_ddrenorm(ph, pl);
					stbsp_ddmulthi(p2h, p2l, ph, stbsp_bot[e]);
					stbsp_ddmultlos(p2h, p2l, stbsp_bot[e], pl);
					ph = p2h;
					pl = p2l;
				}
			}
			if (et) {
				stbsp_ddrenorm(ph, pl);
				--et;
				stbsp_ddmulthi(p2h, p2l, ph, stbsp_top[et]);
				stbsp_ddmultlo(p2h, p2l, ph, pl, stbsp_top[et], stbsp_toperr[et]);
				ph = p2h;
				pl = p2l;
			}
		}
	}
	stbsp_ddrenorm(ph, pl);
	*ohi = ph;
	*olo = pl;
}

// given a float value, returns the significant bits in bits, and the position of the
//   decimal point in decimal_pos.  +/-INF and NAN are specified by special values
//   returned in the decimal_pos parameter.
// frac_digits is absolute normally, but if you want from first significant digits (got %g and %e), or in 0x80000000
static stbsp_int32 stbsp_real_to_str(const char **start, stbsp_uint32 *len, char *out, stbsp_int32 *decimal_pos, double value, stbsp_uint32 frac_digits) {
	double d;
	stbsp_int64 bits = 0;
	stbsp_int32 expo, e, ng, tens;

	d = value;
	STBSP_COPYFP(bits, d);
	expo = (stbsp_int32)((bits >> 52) & 2047);
	ng = (stbsp_int32)((stbsp_uint64)bits >> 63);
	if (ng)
		d = -d;

	if (expo == 2047) // is nan or inf?
	{
		*start = (bits & ((((stbsp_uint64)1) << 52) - 1)) ? "NaN" : "Inf";
		*decimal_pos = STBSP_SPECIAL;
		*len = 3;
		return ng;
	}

	if (expo == 0) // is zero or denormal
	{
		if (((stbsp_uint64)bits << 1) == 0) // do zero
		{
			*decimal_pos = 1;
			*start = out;
			out[0] = '0';
			*len = 1;
			return ng;
		}
		// find the right expo for denormals
		{
			stbsp_int64 v = ((stbsp_uint64)1) << 51;
			while ((bits & v) == 0) {
				--expo;
				v >>= 1;
			}
		}
	}

	// find the decimal exponent as well as the decimal bits of the value
	{
		double ph, pl;

		// log10 estimate - very specifically tweaked to hit or undershoot by no more than 1 of log10 of all expos 1..2046
		tens = expo - 1023;
		tens = (tens < 0) ? ((tens * 617) / 2048) : (((tens * 1233) / 4096) + 1);

		// move the significant bits into position and stick them into an int
		stbsp_raise_to_power10(&ph, &pl, d, 18 - tens);

		// get full as much precision from double-double as possible
		stbsp_ddtoS64(bits, ph, pl);

		// check if we undershot
		if (((stbsp_uint64)bits) >= stbsp_tento19th)
			++tens;
	}

	// now do the rounding in integer land
	frac_digits = (frac_digits & 0x80000000) ? ((frac_digits & 0x7ffffff) + 1) : (tens + frac_digits);
	if ((frac_digits < 24)) {
		stbsp_uint32 dg = 1;
		if ((stbsp_uint64)bits >= stbsp_powten[9])
			dg = 10;
		while ((stbsp_uint64)bits >= stbsp_powten[dg]) {
			++dg;
			if (dg == 20)
				goto noround;
		}
		if (frac_digits < dg) {
			stbsp_uint64 r;
			// add 0.5 at the right position and round
			e = dg - frac_digits;
			if ((stbsp_uint32)e >= 24)
				goto noround;
			r = stbsp_powten[e];
			bits = bits + (r / 2);
			if ((stbsp_uint64)bits >= stbsp_powten[dg])
				++tens;
			bits /= r;
		}
	noround:;
	}

	// kill long trailing runs of zeros
	if (bits) {
		stbsp_uint32 n;
		for (;;) {
			if (bits <= 0xffffffff)
				break;
			if (bits % 1000)
				goto donez;
			bits /= 1000;
		}
		n = (stbsp_uint32)bits;
		while ((n % 1000) == 0)
			n /= 1000;
		bits = n;
	donez:;
	}

	// convert to string
	out += 64;
	e = 0;
	for (;;) {
		stbsp_uint32 n;
		char *o = out - 8;
		// do the conversion in chunks of U32s (avoid most 64-bit divides, worth it, constant denomiators be damned)
		if (bits >= 100000000) {
			n = (stbsp_uint32)(bits % 100000000);
			bits /= 100000000;
		} else {
			n = (stbsp_uint32)bits;
			bits = 0;
		}
		while (n) {
			out -= 2;
			*(stbsp_uint16 *)out = *(stbsp_uint16 *)&stbsp_digitpair.pair[(n % 100) * 2];
			n /= 100;
			e += 2;
		}
		if (bits == 0) {
			if ((e) && (out[0] == '0')) {
				++out;
				--e;
			}
			break;
		}
		while (out != o) {
			*--out = '0';
			++e;
		}
	}

	*decimal_pos = tens;
	*start = out;
	*len = e;
	return ng;
}

#endif // STB_SPRINTF_NOFLOAT
