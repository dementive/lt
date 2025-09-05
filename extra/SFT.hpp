#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat"
#pragma clang diagnostic ignored "-Wformat-extra-args"

#include <cstdio>
#include "lt/defs/for_each.hpp"

#define TEST_PRINT_FUNCTION printf
#define PRINT_TEST(message, name, file, line, condition) TEST_PRINT_FUNCTION(message, name, file, line, condition);

#define TEST_CASE_NAME_MESSAGE "\n%s\n"
#define TEST_FAIL_MESSAGE "%-55s | Failed %s%s - %s\n"
#define TEST_PASS_MESSAGE "%-55s | Passed %s%s%s\n"
#define TEST_FAIL_UNNAMED_MESSAGE "%-55d | Failed %s%s - %s\n"
#define TEST_PASS_UNNAMED_MESSAGE "%-55d | Passed %s%s%s\n"

#define STRINGIZE2(x) #x
#define STRINGIZE(x) STRINGIZE2(x)

inline bool SFT_check_result = false;
#define TEST_MESSAGE SFT_check_result ? TEST_PASS_MESSAGE : TEST_FAIL_MESSAGE
#define TEST_UNNAMED_MESSAGE SFT_check_result ? TEST_PASS_UNNAMED_MESSAGE : TEST_FAIL_UNNAMED_MESSAGE

#define TEST_FILE(condition) SFT_check_result ? "" : __FILE__
#define TEST_LINE(condition) SFT_check_result ? "" : ":" STRINGIZE(__LINE__)
#define TEST_CONDITION(condition) SFT_check_result ? "" : #condition
inline int SFT_check_number = 1;

#define named_tests(test_case_name, ...)                                                                                                                                                     \
	TEST_PRINT_FUNCTION(TEST_CASE_NAME_MESSAGE, test_case_name);                                                                                                                             \
	FOR_EACH_THREE(CHECK_NAMED, test_case_name, __VA_OPT__(__VA_ARGS__, ))

#define tests(test_case_name, ...)                                                                                                                                                           \
	SFT_check_number = 1;                                                                                                                                                                    \
	TEST_PRINT_FUNCTION(TEST_CASE_NAME_MESSAGE, test_case_name);                                                                                                                             \
	FOR_EACH_TWO(CHECK, test_case_name, __VA_OPT__(__VA_ARGS__, ))

#define CHECK(test_case_name, condition)                                                                                                                                                     \
	SFT_check_result = condition;                                                                                                                                                            \
	PRINT_TEST(TEST_UNNAMED_MESSAGE, SFT_check_number, TEST_FILE(condition), TEST_LINE(condition), TEST_CONDITION(condition))                                                             \
	SFT_check_number++;

#define CHECK_NAMED(test_case_name, check_name, condition)                                                                                                                                   \
	SFT_check_result = condition;                                                                                                                                                            \
	PRINT_TEST(TEST_MESSAGE, check_name, TEST_FILE(condition), TEST_LINE(condition), TEST_CONDITION(condition))
