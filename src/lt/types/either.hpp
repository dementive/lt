#pragma once

#include "lt/defs/move.hpp"

namespace lt {

// A type that is either one type or another type
template <typename T, typename S> class [[nodiscard]] either {
private:
	union {
		T first_val;
		S second_val;
	};
	bool has_first;

public:
	constexpr either() = delete /*("either must be constructed from a value.")*/;
	constexpr either(T &&p_val) :
			first_val(LT_MOV(p_val)),
			has_first(true) {}
	constexpr either(S &&p_second_val) :
			second_val(LT_MOV(p_second_val)),
			has_first(false) {}

	constexpr operator bool() const { return has_first; }

	constexpr bool is_first() { return has_first ? true : false; }
	constexpr bool is_second() { return !has_first ? true : false; }

	constexpr T &first() { return first_val; }
	constexpr T &first_or() {
		if (has_first)
			return first_val;

		return {};
	}

	constexpr S &second() { return second_val; }
	constexpr S &second_or() {
		if (!has_first)
			return second_val;

		return {};
	}
};

} // namespace lt
