#pragma once

#include "lt/defs/crash.hpp"
#include "lt/defs/move.hpp"

#ifndef LT_ERROR_ENUM_TYPE
#include "lt/types/error.hpp"
#define LT_ERROR_ENUM_TYPE lt::Error
#endif

namespace lt {

// A type that is either a value or an error
// Similar api to rust Result types: https://doc.rust-lang.org/std/result/index.html#extracting-contained-values
template <typename T> struct [[nodiscard]] result {
private:
	using E = LT_ERROR_ENUM_TYPE;
	union {
		T val;
		E err_val;
	};
	bool has_val;

public:
	constexpr result() = delete /*("result must be constructed from either a value or an error.")*/;
	constexpr result(T &&p_val) :
			val(LT_MOV(p_val)),
			has_val(true) {}
	constexpr result(const T &p_val) :
			val(p_val),
			has_val(true) {}

	constexpr result(E p_err_val) :
			err_val(p_err_val),
			has_val(false) {}

	result &operator=(const result &) = delete;
	result &operator=(result &&) = delete;

	constexpr operator bool() const { return has_val; }

	constexpr bool ok() const {
		if (has_val)
			return true;
		return false;
	}
	constexpr bool err() const {
		if (!has_val)
			return true;
		return false;
	}

	constexpr const T &value() const { return val; }
	constexpr const T &expect() const {
		if (!has_val)
			lt_crash("result has no value")
		return val;
	}
	constexpr const T &unwrap() const {
		if (!has_val)
			lt_crash("result has no value")
		return val;
	}
	constexpr T value_or(const T &p_val) const {
		if (has_val)
			return value();
		return p_val;
	}
	constexpr T value_or_default() const {
		if (has_val)
			return value();
		return {};
	}

	constexpr E expect_err() const {
		if (has_val)
			lt_crash("result has has a value. expected an error.")
		return err_val;
	}
	constexpr E unwrap_err() const {
		if (has_val)
			lt_crash("result has has a value. expected an error.")
		return err_val;
	}
	constexpr E error() const { return err_val; }
	constexpr E error_or_default() const {
		if (!has_val)
			return err_val;
		return {};
	}
};

#undef LT_ERROR_ENUM_TYPE

} // namespace lt
