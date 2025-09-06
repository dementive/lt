#include "lt/defs/test.hpp"
#include "lt/types/either.hpp"

using either_t = lt::either<int, const char *>;
static constexpr inline either_t et{ 1 };
static constexpr inline either_t et2{ "Hello World" };

test(either,
	"is_left", et.is_left(),
	"is_right", et2.is_right(),
	"left", et.left() == 1,
	"left_or", et2.left_or(2) == 2,
	"left_or_default", et2.left_or_default() == 0,
	"right", et2.right() != nullptr,
	"right_or", et.right_or("Hi") != nullptr,
	"right_or_default", et2.right_or_default() != nullptr
)
