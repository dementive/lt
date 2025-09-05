#include "lt/defs/test.hpp"
#include "lt/fixed_vector.hpp"

test(fixed_vector,
	lt::fixed_vector<int, 6> vec = {1,2,3,4,5};
	vec[3] = 999;

	named_tests("fixed_vector",
		"operator[]", vec[2] == 3,
		"operator[] assignment", vec[3] == 999,
		"at", vec[4] == 5,

		"size", vec.size() == 5,
		"begin", *vec.begin() == 1,
		"end", *(vec.end() - 1) == 5,

		"push_back", vec.push_back(6); vec.size() == 6,
		"pop_back", vec.pop_back(); vec.size() == 5,
		"front", vec.front() == 1,
		"back", vec.back() == 5
	)
)
