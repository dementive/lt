#include "lt/defs/test.hpp"
#include "lt/types/either.hpp"
#include "lt/types/option.hpp"
#include "lt/types/result.hpp"

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


inline lt::option<int> opt{ 1 };
inline lt::option<int> opt2{};

test(option,
	"some", opt.some(),
	"none", !opt.none(),
	"value", opt.value() == 1,
	"value_or", opt2.value_or(999) == 999,
	"value_or_default", opt2.value_or_default() == 0
)


inline lt::result<int> res{ 1 };
inline lt::result<int> res2{ lt::Error::Err };

test(result,
	"ok", res.ok(),
	"err", !res.err(),
	"value", res.value() == 1,
	"expect", res.expect() == 1,
	"unwrap", res.unwrap() == 1,
	"value_or", res2.value_or(999) == 999,
	"value_or_default", res2.value_or_default() == 0
)
