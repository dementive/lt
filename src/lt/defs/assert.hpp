#pragma once

#include "log.hpp"

#define lt_impl_log_assert_fail(m_cond, m_msg) lt::log("[%s:%s:%d] %s: %s", __FILE__, __FUNCTION__, __LINE__, m_msg, #m_cond)

#define lt_impl_assert(m_cond, m_msg)                                                                                                                                                        \
	if (!(m_cond)) [[unlikely]] {                                                                                                                                                            \
		lt_impl_log_assert_fail(m_cond, m_msg);                                                                                                                                              \
		lt_crash("")                                                                                                                                                                         \
	}                                                                                                                                                                                        \
	((void)0)

#define lt_assert(m_cond) lt_impl_assert(m_cond, "Assert failed")
#define lt_assert_idx(m_cond) lt_impl_assert(m_cond, "Index out of bounds")
#define lt_assert_null(m_cond) lt_impl_assert(m_cond != nullptr, #m_cond " is null")
#define lt_assert_msg(m_cond, m_msg) lt_impl_assert(m_cond, m_msg)

#if defined(_MSC_VER)
#define lt_crash(m_msg) __fastfail(7)
#elif defined(__i386__) || defined(__x86_64__)
#define lt_crash(m_msg) __asm__ volatile("ud2");
#elifdef __thumb__
#define lt_crash(m_msg) __asm__ volatile(".inst 0xde01");
#elifdef __aarch64__
#define lt_crash(m_msg) __asm__ volatile(".inst 0xd4200000");
#elifdef __arm__
#define lt_crash(m_msg) __asm__ volatile(".inst 0xe7f001f0");
#elifdef __riscv
#define lt_crash(m_msg) __asm__ volatile(".4byte 0x00100073");
#elif defined __powerpc__ && !defined _AIX
#define lt_crash(m_msg) __asm__ volatile(".4byte 0x7d821008");
#else
#define lt_crash(m_msg) __builtin_trap();
#endif
