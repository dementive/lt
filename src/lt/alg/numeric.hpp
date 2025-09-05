#pragma once

#include "lt/span.hpp"

namespace lt {

// Calculates the sum of all elements in the span.
template <typename T> constexpr T sum(const span<T> &p_span) {
	T total = T();
	for (int i = 0; i < p_span.size(); i++)
		total += p_span.ptr[i];
	return total;
}

// Finds the maximum element in the span.
template <typename T> constexpr const T &max(const span<T> &p_span) {
	if (p_span.size() == 0)
		return T();
	const T *max_val = &p_span.ptr[0];
	for (int i = 1; i < p_span.size(); i++)
		if (p_span.ptr[i] > *max_val)
			max_val = &p_span.ptr[i];
	return *max_val;
}

// Finds the smallest element in the span.
template <typename T> constexpr const T &min(const span<T> &p_span) {
	if (p_span.size() == 0)
		return T();
	const T *min_val = &p_span.ptr[0];
	for (int i = 1; i < p_span.size(); i++)
		if (p_span.ptr[i] < *min_val)
			min_val = &p_span.ptr[i];
	return *min_val;
}

// Returns the number of elements equal to p_val
template <typename T> constexpr int count(const span<T> &p_span, const T &p_val) {
	int amount = 0;
	for (int i = 0; i < p_span.size(); i++)
		if (p_span.ptr[i] == p_val)
			amount++;
	return amount;
}

// Sums up or folds a range of elements
template <class T> constexpr T accumulate(const span<T> &p_span, T init) {
	T *first = p_span.begin();
	T *last = p_span.end();
	for (; first != last; ++first)
		init = LT_MOV(init) + *first;
	return init;
}

// Checks if two spans have equal contents.
template <typename T> constexpr bool equal(const span<T> &p_span1, const span<T> &p_span2) {
	if (p_span1.size() != p_span2.size())
		return false;
	for (int i = 0; i < p_span1.size(); i++)
		if (p_span1.ptr[i] != p_span2.ptr[i])
			return false;
	return true;
}

} // namespace lt
