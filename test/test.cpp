#include "../extra/SFT.hpp"

// Tests to test the tests
void test_tests() {
	tests("SFT",
		0 == 1, // test fail
		999 == 999 // test pass
	)

	named_tests("SFT",
		"Fail test", 0 == 1,
		"Pass test", 999 == 999
	)
}