#pragma once

#include "lt/types/range.hpp"

namespace lt {

// Reverses the order of the elements in the range
template <typename T> constexpr void reverse(Range auto &p_range) {
	int start = 0;
	int end = p_range.size() - 1;
	while (start < end) {
		T temp = p_range[start];
		p_range[start] = p_range[end];
		p_range[end] = temp;
		start++;
		end--;
	}
}

// Copy assigns the given value to every element in a range
template <typename T> constexpr void fill(Range auto &p_range, const T &p_value) {
	T *first = p_range.begin();
	T *last = p_range.end();
	for (; first != last; ++first)
		*first = p_value;
}

// Fills a range with successive increments of the starting value
template <class T> constexpr void iota(Range auto &p_range, T value) {
	T *first = p_range.begin();
	T *last = p_range.end();
	for (; first != last; ++first, ++value)
		*first = value;
}

// Replaces all values satisfying specific criteria with another value
template <typename T> constexpr void replace(Range auto &p_range, const T &old_value, const T &new_value) {
	T *first = p_range.begin();
	T *last = p_range.end();
	for (; first != last; ++first)
		if (*first == old_value)
			*first = new_value;
}

} // namespace lt
