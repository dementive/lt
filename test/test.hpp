#include "lt/defs/test.hpp"

// Tests to test the tests
test(tests,
	tests("tests",
		0 == 1, // test fail
		999 == 999 // test pass
	)
	named_tests("tests",
		"Fail test", 0 == 1,
		"Pass test", 999 == 999
	)
)
