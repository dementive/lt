#pragma once

#include "lt/defs/move.hpp"

namespace lt {

// A type that is either one type or another type
template <typename T, typename S> class [[nodiscard]] either {
private:
	union {
		T left_val;
		S right_val;
	};
	bool has_left;

public:
	constexpr either() = delete /*("either must be constructed from a value.")*/;
	constexpr either(T &&p_val) :
			left_val(LT_MOV(p_val)),
			has_left(true) {}
	constexpr either(S &&p_right_val) :
			right_val(LT_MOV(p_right_val)),
			has_left(false) {}

	constexpr const T &left() const { return left_val; }
	constexpr bool is_left() const { return has_left ? true : false; }
	constexpr T left_or(const T &p_val) const { return has_left ? left_val : p_val; }
	constexpr T left_or_default() const { return has_left ? left_val : T(); }

	constexpr const S &right() const { return right_val; }
	constexpr bool is_right() const { return !has_left ? true : false; }
	constexpr S right_or(const S &p_val) const { return !has_left ? right_val : p_val; }
	constexpr S right_or_default() const { return !has_left ? right_val : S(); }
};

} // namespace lt
