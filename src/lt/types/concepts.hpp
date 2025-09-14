#pragma once

namespace lt {

template <typename T>
concept Spanable = requires(T p_t) {
	{ p_t.begin() };
	{ p_t.end() };
	{ p_t.size() };
};

} // namespace lt
