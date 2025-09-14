#include "lt/alg/search.hpp"
#include "lt/defs/test.hpp"
#include "lt/array.hpp"

inline lt::array<int, 5> data = { 1, 2, 3, 4, 5 };
inline lt::array<int, 1> empty_data = {0};

inline auto is_even = [](int val) { return val % 2 == 0; };
inline auto is_greater_than_3 = [](int val) { return val > 3; };
inline auto is_negative = [](int val) { return val < 0; };

test(search,
	"find existing value", lt::find(data, 3),
	"find non-existing value", lt::find(data, 99) == nullptr,
	"find on empty span", lt::find(empty_data, 1) == nullptr,
	"find result value", *lt::find(data, 3) == 3,

	"find_last existing value", lt::find_last(data, 3),
	"find_last non-existing value", lt::find_last(data, 99) == nullptr,
	"find_last on empty span", lt::find_last(empty_data, 1) == nullptr,
	"find_last result value", *lt::find_last(data, 3) == 3,

	"find_if existing value", lt::find_if<int>(data, is_even),
	"find_if non-existing value", lt::find_if<int>(data, is_negative) == nullptr,
	"find_if result value", *lt::find_if<int>(data, is_even) == 2,

	"find_if_not existing value", lt::find_if_not<int>(data, is_even),
	"find_if_not non-existing value", lt::find_if_not<int>(data, is_negative),
	"find_if_not result value", *lt::find_if_not<int>(data, is_even) == 1,

	"all_of all true", lt::all_of<int>(data, is_greater_than_3) == false,
	"all_of all false", lt::all_of<int>(data, is_negative) == false,
	"all_of with true predicate", lt::all_of<int>(data, [](int val){ return val > 0; }) == true,
	"all_of on empty span", lt::all_of<int>(empty_data, is_even) == true,

	"any_of existing true", lt::any_of<int>(data, is_even) == true,
	"any_of no true", lt::any_of<int>(data, is_negative) == false,
	"any_of on empty span", lt::any_of<int>(empty_data, is_even) == true,

	"none_of no true", lt::none_of<int>(data, is_negative) == true,
	"none_of existing true", lt::none_of<int>(data, is_even) == false,
	"none_of on empty span", lt::none_of<int>(empty_data, is_even) == false
)
