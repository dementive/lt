#include "lt/const_map.hpp"
#include "lt/defs/test.hpp"
#include "lt/fixed_string.hpp"

enum Props : unsigned char { Yes, No, PROP_MAX };

test(const_map,
	static constexpr lt::const_map<lt::cstr, lt::cstr, PROP_MAX> map{
		{ "yes", "cowabunga" },
		{ "no", "zoowemomma" }
	};

	named_tests("const_map",
		"at", map.at("yes") == "cowabunga",
		"Key operator[]", map["no"] == "zoowemomma",
		"Index operator[]", map[0] == "cowabunga",
		"key_at_index", map.key_at_index(0) == "yes",
		"value_at_index", map.value_at_index(0) == "cowabunga",
		"size", map.size() == 2
	)
)
