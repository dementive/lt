#include "lt/alg/numeric.hpp"
#include "lt/defs/test.hpp"
#include "lt/array.hpp"

inline lt::array<int, 5> int_data = { 1, 2, 3, 4, 5 };
inline lt::array<int, 5> int_data_with_duplicates = { 1, 2, 2, 4, 5 };
inline lt::array<int, 5> int_data_negative = { -1, -2, -3, -4, -5 };
inline lt::array<int, 5> int_data_all_same = { 7, 7, 7, 7, 7 };
inline lt::array<int, 1> int_empty_data = {0};

inline lt::array<float, 5> float_data = { 1.1F, 2.2F, 3.3F, 4.4F, 5.5F };

using NumArray = lt::array<int, 5>;

test(numeric,
	"sum of positive integers", lt::sum<int>(int_data) == 15,
	"sum of negative integers", lt::sum<int>(int_data_negative) == -15,
	"sum of empty span", lt::sum<int>(int_empty_data) == 0,
	"sum of floats", lt::sum<float>(float_data) == 16.5F,

	"max of positive integers", *lt::max<int>(int_data) == 5,
	"max with negative integers", *lt::max<int>(int_data_negative) == -1,
	"max with duplicates", *lt::max<int>(int_data_with_duplicates) == 5,
	"max on empty span", *lt::max<int>(int_empty_data) == int(),

	"min of positive integers", *lt::min<int>(int_data) == 1,
	"min with negative integers", *lt::min<int>(int_data_negative) == -5,
	"min with duplicates", *lt::min<int>(int_data_with_duplicates) == 1,
	"min on empty span", *lt::min<int>(int_empty_data) == int(),

	"count of existing value", lt::count(int_data, 3) == 1,
	"count of duplicate value", lt::count(int_data_with_duplicates, 2) == 2,
	"count of non-existing value", lt::count(int_data, 99) == 0,
	"count on empty span", lt::count(int_empty_data, 1) == 0,

	"accumulate positive integers", lt::accumulate(int_data, 0) == 15,
	"accumulate with initial value", lt::accumulate(int_data, 10) == 25,
	"accumulate on empty span", lt::accumulate(int_empty_data, 5) == 5,
	"accumulate with floats", lt::accumulate(float_data, 0.0F) == 16.5F,

	"equal same spans", lt::equal<int>(int_data, int_data) == true,
	"equal different spans", lt::equal<int>(int_data, int_data_with_duplicates) == false,
	"equal spans of different sizes", lt::equal<int>(int_data, NumArray({1,2,3,4})) == false,
	"equal empty spans", lt::equal<int>(int_empty_data, int_empty_data) == true
)
