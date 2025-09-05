#pragma once

namespace lt::detail {

template <typename T>
concept Spanable = requires(T p_t) {
	{ p_t.begin() };
	{ p_t.size() };
};

} // namespace lt::detail
