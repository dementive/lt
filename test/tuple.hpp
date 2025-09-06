#include "lt/defs/test.hpp"
#include "lt/tuple.hpp"

struct TestStruct {};
inline lt::tuple<int, double, float, TestStruct, int> tup{ 1, 2.0, 3.0, TestStruct(), 999 };

test(tuple,
	"get", lt::get<0>(tup) == 1 and lt::get<1>(tup) == 2.0 and lt::get<4>(tup) == 999
)
