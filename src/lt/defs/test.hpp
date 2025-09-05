#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"
#pragma clang diagnostic ignored "-Wformat-extra-args"

#include "lt/defs/for_each.hpp"

#include <cstdio>

// Usage interface

#define test(m_name, ...)                                                                                                                                                                    \
	static inline void test_##m_name() { __VA_ARGS__ }

#define named_tests(test_case_name, ...)                                                                                                                                                     \
	TEST_PRINT_FUNCTION(TEST_CASE_NAME_MESSAGE, test_case_name);                                                                                                                             \
	FOR_EACH_THREE(CHECK_NAMED, test_case_name, __VA_OPT__(__VA_ARGS__, ))

#define tests(test_case_name, ...)                                                                                                                                                           \
	LT_check_number = 1;                                                                                                                                                                     \
	TEST_PRINT_FUNCTION(TEST_CASE_NAME_MESSAGE, test_case_name);                                                                                                                             \
	FOR_EACH_TWO(CHECK, test_case_name, __VA_OPT__(__VA_ARGS__, ))

#define run_tests(...)                                                                                                                                                                       \
	FOR_EACH(RUN_TEST, __VA_OPT__(__VA_ARGS__, ))                                                                                                                                            \
	TEST_END_MESSAGE()

#define test_main(...)                                                                                                                                                                       \
	int main(int argc, const char *argv[]) {                                                                                                                                                 \
		run_tests(__VA_ARGS__);                                                                                                                                                              \
		TEST_END_MESSAGE()                                                                                                                                                                   \
	}

/*
clang-format will completely destroy the above macros when used.
To fix this without littering every test file with "clang-format off" use the following settings in your .clang-format file:

WhitespaceSensitiveMacros:
  - test
  - named_tests
  - tests
*/

// Impl

#define TEST_PRINT_FUNCTION printf
#define PRINT_TEST(message, name, file, line, condition) TEST_PRINT_FUNCTION(message, name, file, line, condition);

#define TEST_CASE_NAME_MESSAGE "\n%s\n"
#define TEST_FAIL_MESSAGE "%-55s | Failed %s%s - %s\n"
#define TEST_PASS_MESSAGE "%-55s | Passed %s%s%s\n"
#define TEST_FAIL_UNNAMED_MESSAGE "%-55d | Failed %s%s - %s\n"
#define TEST_PASS_UNNAMED_MESSAGE "%-55d | Passed %s%s%s\n"
#define TEST_END_MESSAGE() TEST_PRINT_FUNCTION("\n--------------------\n%d Tests\n%d Passed\n%d Failed", lt_total_tests, lt_passed_tests, lt_failed_tests);

#define STRINGIZE2(x) #x
#define STRINGIZE(x) STRINGIZE2(x)

static inline int lt_total_tests = 0;
static inline int lt_passed_tests = 0;
static inline int lt_failed_tests = 0;

#define TEST_LOG_CHECK_RESULT(condition)                                                                                                                                                     \
	if (condition)                                                                                                                                                                           \
		lt_passed_tests++;                                                                                                                                                                   \
	else                                                                                                                                                                                     \
		lt_failed_tests++;                                                                                                                                                                   \
	lt_total_tests++;

thread_local inline bool LT_check_result = false;
#define TEST_MESSAGE LT_check_result ? TEST_PASS_MESSAGE : TEST_FAIL_MESSAGE
#define TEST_UNNAMED_MESSAGE LT_check_result ? TEST_PASS_UNNAMED_MESSAGE : TEST_FAIL_UNNAMED_MESSAGE

#define TEST_FILE(condition) LT_check_result ? "" : __FILE__
#define TEST_LINE(condition) LT_check_result ? "" : ":" STRINGIZE(__LINE__)
#define TEST_CONDITION(condition) LT_check_result ? "" : #condition
inline int LT_check_number = 1;

#define CHECK(test_case_name, condition)                                                                                                                                                     \
	LT_check_result = condition;                                                                                                                                                             \
	TEST_LOG_CHECK_RESULT(condition)                                                                                                                                                         \
	PRINT_TEST(TEST_UNNAMED_MESSAGE, LT_check_number, TEST_FILE(condition), TEST_LINE(condition), TEST_CONDITION(condition))                                                                 \
	LT_check_number++;

#define CHECK_NAMED(test_case_name, check_name, condition)                                                                                                                                   \
	LT_check_result = condition;                                                                                                                                                             \
	TEST_LOG_CHECK_RESULT(condition)                                                                                                                                                         \
	PRINT_TEST(TEST_MESSAGE, check_name, TEST_FILE(condition), TEST_LINE(condition), TEST_CONDITION(condition))

#define RUN_TEST(test_case_name) test_##test_case_name();
