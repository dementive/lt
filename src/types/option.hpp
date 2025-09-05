#pragma once

#include <new> // for placement new

#include "lt/defs/move.hpp"

namespace lt {

// option represents an optional value: every option is either Some and contains a value, or None, and does not.
template <typename T> struct [[nodiscard]] option {
private:
	alignas(T) unsigned char storage[sizeof(T)];
	bool has_val{};

public:
	constexpr option() = default;

	constexpr option(const T &p_val) :
			has_val(true) {
		new (storage) T(p_val);
	}

	constexpr option(T &&p_val) :
			has_val(true) {
		new (storage) T(LT_MOV(p_val));
	}

	constexpr ~option() {
		if (has_val)
			reinterpret_cast<T *>(storage)->~T();
	}

	option &operator=(const option &) = delete;
	option &operator=(option &&) = delete;

	constexpr operator bool() const { return has_val; }
	constexpr bool some() const { return has_val; }
	constexpr bool none() const { return !has_val; }

	constexpr T &value() { return *reinterpret_cast<T *>(storage); }
	constexpr const T &value() const { return *reinterpret_cast<const T *>(storage); }

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
};

} // namespace lt
