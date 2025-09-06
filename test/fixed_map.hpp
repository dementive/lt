#include "lt/defs/test.hpp"
#include "lt/fixed_map.hpp"
#include "lt/fixed_string.hpp"

enum class Props2 : unsigned char { Yes, No, PROP_MAX };
static lt::fixed_map<lt::cstr, lt::cstr, int(Props2::PROP_MAX) + 1> fmap{
	{ .first = "yes", .second = "cowabunga" },
	{ .first = "no", .second = "zoowemomma" }
};

test(fixed_map,
	"size", fmap.size() == 2,
	"insert", fmap.insert("hi", "hey"); fmap.size() == 3,
	"operator[]", fmap["hi"] == "hey",
	"key_at_index", fmap.key_at_index(0) == "yes",
	"value_at_index", fmap.value_at_index(0) == "cowabunga",
	"insert", fmap.erase("hi"); fmap.size() == 2,
	"has", fmap.has("yes"),
	"find", fmap.find("yes") != nullptr,
	"begin", fmap.begin()->first == "yes",
	"end", (fmap.end() - 1)->first == "no"
)
