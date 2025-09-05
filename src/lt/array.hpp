#pragma once

#include "lt/defs/crash.hpp"

#include <initializer_list>
#include <stddef.h>

namespace lt {

// Simple wrapper around a C array.
template <typename T, size_t N> struct array {
private:
	using Type = T[N];

public:
	Type data;

	constexpr size_t size() { return N; }
	constexpr const T &operator[](size_t p_index) const { return data[p_index]; }
	constexpr T &operator[](size_t p_index) { return data[p_index]; }

	constexpr const T &at(size_t p_index) const {
		if (p_index >= N)
			lt_crash("index out of bounds");

		return data[p_index];
	}
	constexpr T &at(size_t p_index) {
		if (p_index >= N)
			lt_crash("index out of bounds");

		return data[p_index];
	}

	constexpr array() = default;
	constexpr array(std::initializer_list<T> p_init) {
		if (p_init.size() != N)
			lt_crash("index out of bounds");

		for (int i = 0; const T &E : p_init)
			data[i++] = E;
	};

	constexpr T *begin() { return data; }
	constexpr T *end() { return data + N; }
	constexpr const T *begin() const { return data; }
	constexpr const T *end() const { return data + N; }
};

} // namespace lt
