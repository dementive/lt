#include "lt/defs/test.hpp"
#include "lt/fixed_string.hpp"

inline lt::cstr test_str{ "Hello World" };

test(fixed_string,
	"operator==(const char *other)", test_str == "Hello World",
	"operator!=(const char *other)", test_str != "Hey World",
	"operator=(const char *other)", test_str = "Hello"; test_str == "Hello",
	"c_str", test_str.c_str() != nullptr,
	"length", test_str = "Hello World"; test_str.length() == 11,
	"max_size", test_str.max_size() == 15,
	"substr", test_str.substr(6) == "World",
	"ends_with", test_str.ends_with("orld")
)
