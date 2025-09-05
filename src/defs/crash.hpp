#pragma once

#if defined(_MSC_VER)
#define lt_crash(m_msg) __debugbreak();
#elif defined(__i386__) || defined(__x86_64__)
#define lt_crash(m_msg) __asm__ volatile("ud2");
// #define lt_crash(m_msg) __asm__ volatile ("int $0x3\n\tnop");
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
