#include "lt/defs/test.hpp"
#include "lt/fixed_set.hpp"

inline lt::fixed_set<int, 6> set = { 1, 2, 3, 4, 5 };

test(fixed_set,
	"operator[]", set[2] == 3,
	"operator[] assignment", set[3] = 999; set[3] == 999,
	"at", set[4] == 5,
	"size", set.size() == 5,
	"push_back", set.push_back(6); set.size() == 6,
	"pop_back", set.pop_back(); set.size() == 5,
	"front", set.front() == 1,
	"back", set.back() == 5,
	"begin", *set.begin() == 1,
	"end", *(set.end() - 1) == 5,
	"has", set.has(1),
	"erase", set.erase(1); !set.has(1)
)
