#pragma once

#include "lt/range.hpp"

namespace lt {

// Reverses the order of the elements in the range
template <typename T> constexpr void reverse(const range<T> &p_range) {
	int start = 0;
	int end = p_range.size() - 1;
	while (start < end) {
		T temp = p_range.ptr[start];
		p_range.ptr[start] = p_range.ptr[end];
		p_range.ptr[end] = temp;
		start++;
		end--;
	}
}

// Copy assigns the given value to every element in a range
template <typename T> constexpr void fill(const range<T> &p_range, const T &p_value) {
	T *first = p_range.begin();
	T *last = p_range.end();
	for (; first != last; ++first)
		*first = p_value;
}

// Fills a range with successive increments of the starting value
template <class T> constexpr void iota(const range<T> &p_range, T value) {
	T *first = p_range.begin();
	T *last = p_range.end();
	for (; first != last; ++first, ++value)
		*first = value;
}

// Replaces all values satisfying specific criteria with another value
template <typename T> constexpr void replace(const range<T> &p_range, const T &old_value, const T &new_value) {
	T *first = p_range.begin();
	T *last = p_range.end();
	for (; first != last; ++first)
		if (*first == old_value)
			*first = new_value;
}

} // namespace lt
