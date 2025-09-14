#pragma once

#include "lt/defs/move.hpp"
#include "lt/types/range.hpp"

#include <cstddef>

namespace lt {

// Calculates the sum of all elements in a range.
template <typename T> constexpr T sum(const Range auto &p_range) {
	T total = T();
	for (size_t i = 0; i < p_range.size(); i++)
		total += p_range[i];
	return total;
}

// Finds the maximum element in a range.
template <typename T> constexpr const T *max(const Range auto &p_range) {
	if (p_range.size() == 0)
		return {};

	const T *max_val = &p_range[0];
	for (size_t i = 1; i < p_range.size(); i++)
		if (p_range[i] > *max_val)
			max_val = &p_range[i];
	return max_val;
}

// Finds the smallest element in a range.
template <typename T> constexpr const T *min(const Range auto &p_range) {
	if (p_range.size() == 0)
		return {};
	const T *min_val = &p_range[0];
	for (size_t i = 1; i < p_range.size(); i++)
		if (p_range[i] < *min_val)
			min_val = &p_range[i];
	return min_val;
}

// Returns the number of elements equal to p_val
template <typename T> constexpr int count(const Range auto &p_range, const T &p_val) {
	int amount = 0;
	for (size_t i = 0; i < p_range.size(); i++)
		if (p_range[i] == p_val)
			amount++;
	return amount;
}

// Sums up or folds a range of elements
template <class T> constexpr T accumulate(const Range auto &p_range, T init) {
	const T *first = p_range.begin();
	const T *last = p_range.end();
	for (; first != last; ++first)
		init = LT_MOV(init) + *first;
	return init;
}

// Checks if two ranges have equal contents.
template <typename T> constexpr bool equal(const Range auto &p_range1, const Range auto &p_range2) {
	if (p_range1.size() != p_range2.size())
		return false;
	for (size_t i = 0; i < p_range1.size(); i++)
		if (p_range1[i] != p_range2[i])
			return false;
	return true;
}

} // namespace lt
