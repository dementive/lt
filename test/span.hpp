#include "lt/array.hpp"
#include "lt/defs/test.hpp"
#include "lt/span.hpp"

inline lt::array<int, 5> arr1{ { 1, 2, 3, 4, 5 } };
inline lt::span<int> span1{ arr1 };

inline lt::array<float, 2> arr2{ { 1.5, 2.2 } };
inline lt::span<float> span2{ arr2 };
inline auto span3 = to_span<float>(arr2);

test(span,
	"size", span1.size() == 5 and span2.size() == 2,
	"operator[]", span1[2] == 3,
	"at", span1[3] == 4,
	"begin", *span1.begin() == 1,
	"end", *(span1.end() - 1) == 5,
	"subspan", span1.subspan(0, 3).size() == 3,
	"first", span1.first(3).size() == 3,
	"last", span1.last(2).size() == 2
)
