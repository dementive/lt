#pragma once

namespace lt {

template <typename T>
concept Range = requires(T p_t) {
	{ p_t.begin() };
	{ p_t.end() };
	{ p_t.size() };
	{ p_t[0] };
};

} // namespace lt
