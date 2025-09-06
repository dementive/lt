#include "lt/const_map.hpp"
#include "lt/defs/test.hpp"
#include "lt/fixed_string.hpp"

enum class Props : unsigned char { Yes, No, PROP_MAX };
static constexpr lt::const_map<lt::cstr, lt::cstr, size_t(Props::PROP_MAX)> map{ { .first = "yes", .second = "cowabunga" }, { .first = "no", .second = "zoowemomma" } };

test(const_map,
	"at", map.at("yes") == "cowabunga",
	"Key operator[]", map["no"] == "zoowemomma",
	"Index operator[]", map[0] == "cowabunga",
	"key_at_index", map.key_at_index(0) == "yes",
	"value_at_index", map.value_at_index(0) == "cowabunga",
	"size", map.size() == 2,
	"begin", map.begin()->first == "yes",
	"end", (map.end() - 1)->first == "no"
)
