#pragma once

namespace lt {

template <typename F, typename S> struct pair {
	F first{};
	S second{};
	constexpr bool operator==(const pair &p_other) const { return first == p_other.first && second == p_other.second; }
};

} // namespace lt
