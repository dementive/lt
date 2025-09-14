#include "lt/alg/modify.hpp"
#include "lt/defs/test.hpp"
#include "lt/array.hpp"
#include "lt/alg/numeric.hpp"

using ArrayType = lt::array<int, 5>;
using BigArrayType = lt::array<int, 7>;

inline ArrayType test_arr = {1, 2, 3, 4, 5};
inline ArrayType arr_reverse = {5, 4, 3, 2, 1};

test(modify,
	"reverse", test_func(
		lt::reverse<int>({test_arr});
		return lt::equal<int>({arr_reverse}, {arr_reverse});
	),
	"fill", test_func(
		auto x = ArrayType({1,2,3,4,5});
		auto y = ArrayType({9,9,9,9,9});

		lt::fill({x}, 9);
		return lt::equal<int>({x}, {y});
	),
	"iota", test_func(
		auto x = ArrayType({0,0,0,0,0});
		auto y = ArrayType({10,11,12,13,14});

		lt::iota({x}, 10);
		return lt::equal<int>({x}, {y});
	),
	"replace all", test_func(
		auto x = BigArrayType({1, 2, 2, 3, 4, 2, 5});
		auto y = BigArrayType({1, 88, 88, 3, 4, 88, 5});

		lt::replace({x}, 2, 88);
		return lt::equal<int>({x}, {y});
	),
	"replace none", test_func(
		auto x = BigArrayType({1, 2, 2, 3, 4, 2, 5});
		auto y = BigArrayType({1, 2, 2, 3, 4, 2, 5});

		lt::replace({x}, 99, 100);
		return lt::equal<int>({x}, {y});
	)
)
