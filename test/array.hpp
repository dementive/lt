#include "lt/array.hpp"
#include "lt/defs/test.hpp"

inline lt::array<int, 5> arr = { 1, 2, 3, 4, 5 };

test(array,
	"operator[]", arr[2] == 3,
	"operator[] assignment", arr[3] = 999; arr[3] == 999,
	"at", arr[4] == 5,
	"size", arr.size() == 5,
	"begin", *arr.begin() == 1,
	"end", *(arr.end() - 1) == 5
)
