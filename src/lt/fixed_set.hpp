#pragma once

#include "lt/defs/assert.hpp"

#include <initializer_list>

namespace lt {

#ifndef LT_SET_SIZE_TYPE
#define LT_SET_SIZE_TYPE unsigned char
#endif

/*
Fixed Set

the same as lt::fixed_vector except:

- Will not allow pushing of duplicate values
- has() function to check if set has an element.
- erase() function to remove elements.
*/
template <typename T, int N> struct fixed_set {
private:
	using Type = T[N];
	LT_SET_SIZE_TYPE current_size = 0;

public:
	Type data;

	constexpr int size() { return current_size; }
	constexpr const T &operator[](int p_index) const { return data[p_index]; }
	constexpr T &operator[](int p_index) { return data[p_index]; }

	constexpr const T &at(int p_index) const {
		lt_assert_idx(p_index < N);
		return data[p_index];
	}
	constexpr T &at(int p_index) {
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

	constexpr void erase(const T &p_val) {
		int to_remove = 0;
		for (int i = 0; i < current_size; ++i)
			if (data[i] == p_val)
				to_remove = i;

		if (to_remove == current_size - 1) {
			data[to_remove].~T();
			current_size--;
			return;
		}

		data[to_remove] = data[current_size - 1];
		current_size--;
	}

	constexpr bool has(const T &p_val) {
		for (int i = 0; i < current_size; ++i)
			if (data[i] == p_val)
				return true;

		return false;
	}

	constexpr fixed_set() = default;
	constexpr fixed_set(std::initializer_list<T> p_init) {
		for (const T &E : p_init)
			if (!has(E))
				data[current_size++] = E;
	};

	constexpr T *begin() { return data; }
	constexpr T *end() { return data + current_size; }
	constexpr const T *begin() const { return data; }
	constexpr const T *end() const { return data + current_size; }
};

#undef LT_SET_SIZE_TYPE

} // namespace lt
