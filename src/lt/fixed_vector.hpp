#pragma once

#include "lt/defs/assert.hpp"

#include <initializer_list>
#include <stddef.h>

namespace lt {

#ifndef LT_VEC_SIZE_TYPE
#define LT_VEC_SIZE_TYPE unsigned char
#endif

/*
Fixed Vector

the same as lt::array except:

- Has a push_back function to add new elements.
- Has pop_back and back functions
- Keeps track of it's size so iteration only visits elements that have been pushed.
*/
template <typename T, size_t N> struct fixed_vector {
private:
	using Type = T[N];
	LT_VEC_SIZE_TYPE current_size = 0;

public:
	Type data;

	constexpr size_t size() const { return current_size; }
	constexpr const T &operator[](size_t p_index) const { return data[p_index]; }
	constexpr T &operator[](size_t p_index) { return data[p_index]; }

	constexpr const T &at(size_t p_index) const {
		lt_assert_idx(p_index < N);
		return data[p_index];
	}
	constexpr T &at(size_t p_index) {
		lt_assert_idx(p_index < N);
		return data[p_index];
	}

	constexpr void push_back(const T &p_val) {
		lt_assert_idx(current_size <= N);
		data[current_size++] = T(p_val);
	}

	constexpr void pop_back() {
		data[current_size - 1].~T();
		current_size--;
	}
	constexpr const T &back() const { return data[current_size - 1]; }
	constexpr T &back() { return data[current_size - 1]; }

	constexpr const T &front() const { return data[0]; }
	constexpr T &front() { return data[0]; }

	constexpr fixed_vector() = default;
	constexpr fixed_vector(std::initializer_list<T> p_init) {
		for (const T &E : p_init)
			data[current_size++] = E;
	};

	constexpr T *begin() { return data; }
	constexpr T *end() { return data + current_size; }
	constexpr const T *begin() const { return data; }
	constexpr const T *end() const { return data + current_size; }
};

#undef LT_VEC_SIZE_TYPE

} // namespace lt
