#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"
#pragma clang diagnostic ignored "-Wformat-extra-args"

#include "lt/defs/for_each.hpp"

#include <cstdio>

// Usage interface

#define test(m_name, ...) test_func(m_name, named_tests(#m_name,__VA_ARGS__))
#define utest(m_name, ...) test_func(m_name, unamed_tests(#m_name, __VA_ARGS__))

#define test_func(m_name, ...)                                                                                                                                                               \
	static inline void test_##m_name() { __VA_ARGS__ }

#define named_tests(test_case_name, ...)                                                                                                                                                     \
	TEST_PRINT_FUNCTION(TEST_CASE_NAME_MESSAGE, test_case_name);                                                                                                                             \
	FOR_EACH_THREE(CHECK_NAMED, test_case_name, __VA_OPT__(__VA_ARGS__, ))

#define unamed_tests(test_case_name, ...)                                                                                                                                                    \
	LT_check_number = 1;                                                                                                                                                                     \
	TEST_PRINT_FUNCTION(TEST_CASE_NAME_MESSAGE, test_case_name);                                                                                                                             \
	FOR_EACH_TWO(CHECK, test_case_name, __VA_OPT__(__VA_ARGS__, ))

#define run_tests(...) FOR_EACH(RUN_TEST, __VA_OPT__(__VA_ARGS__, ))

#define test_main(...)                                                                                                                                                                       \
	int main(int argc, const char *argv[]) { run_tests(__VA_ARGS__) TEST_END_MESSAGE() return lt_failed_tests > 0 ? 1 : 0; }

/*
clang-format will completely destroy the above macros when used.
To fix this without littering every test file with "clang-format off" use the following settings in your .clang-format file:

WhitespaceSensitiveMacros:
  - test
  - utest
  - named_tests
  - unamed_tests
*/

// Impl

#define TEST_PRINT_FUNCTION printf
#define PRINT_TEST(message, name, file, line, condition) TEST_PRINT_FUNCTION(message, name, file, line, condition);

#define TEST_FAIL_MESSAGE "%-55s - \033[31mFailed\033[0m - %s%s | %s\n"
#define TEST_PASS_MESSAGE "%-55s | \033[32mPassed\033[0m | %s%s%s\n"

#define TEST_FAIL_UNNAMED_MESSAGE "%-55d - \033[31mFailed\033[0m - %s%s | %s\n"
#define TEST_PASS_UNNAMED_MESSAGE "%-55d | \033[32mPassed\033[0m | %s%s%s\n"

#define TEST_CASE_NAME_MESSAGE "\n\033[33m%s\033[0m\n"

#define TEST_END_MESSAGE() TEST_PRINT_FUNCTION("\n------------\n%d Tests\n%d Passed\n%d Failed\n------------\n", lt_total_tests, lt_passed_tests, lt_failed_tests);

#define STRINGIZE2(x) #x
#define STRINGIZE(x) STRINGIZE2(x)

static inline int lt_total_tests = 0;
static inline int lt_passed_tests = 0;
static inline int lt_failed_tests = 0;

#define TEST_DO_CONDITION_CHECK(condition)                                                                                                                                                   \
	if (condition) {                                                                                                                                                                         \
		LT_check_result = true;                                                                                                                                                              \
		lt_passed_tests++;                                                                                                                                                                   \
	} else {                                                                                                                                                                                 \
		LT_check_result = false;                                                                                                                                                             \
		lt_failed_tests++;                                                                                                                                                                   \
	}                                                                                                                                                                                        \
	lt_total_tests++;

thread_local inline bool LT_check_result = false;
#define TEST_MESSAGE LT_check_result ? TEST_PASS_MESSAGE : TEST_FAIL_MESSAGE
#define TEST_UNNAMED_MESSAGE LT_check_result ? TEST_PASS_UNNAMED_MESSAGE : TEST_FAIL_UNNAMED_MESSAGE

#define TEST_FILE LT_check_result ? "" : __FILE__
#define TEST_LINE LT_check_result ? "" : ":" STRINGIZE(__LINE__)
#define TEST_CONDITION(condition) LT_check_result ? "" : #condition
inline int LT_check_number = 1;

#define CHECK(test_case_name, condition)                                                                                                                                                     \
	TEST_DO_CONDITION_CHECK(condition)                                                                                                                                                       \
	PRINT_TEST(TEST_UNNAMED_MESSAGE, LT_check_number, TEST_FILE, TEST_LINE, TEST_CONDITION(condition))                                                                                       \
	LT_check_number++;

#define CHECK_NAMED(test_case_name, check_name, condition)                                                                                                                                   \
	TEST_DO_CONDITION_CHECK(condition)                                                                                                                                                       \
	PRINT_TEST(TEST_MESSAGE, check_name, TEST_FILE, TEST_LINE, TEST_CONDITION(condition))

#define RUN_TEST(test_case_name) test_##test_case_name();
