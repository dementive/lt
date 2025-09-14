// Search and query algorithms
#pragma once

#include "lt/types/range.hpp"

#include <cstddef>

namespace lt {

// Returns the first occurrence of a value, searching from the beginning.
template <typename T> constexpr const T *find(const Range auto &p_range, const T &p_val) {
	for (size_t i = 0; i < p_range.size(); i++)
		if (p_range[i] == p_val)
			return { &p_range[i] };
	return {};
}

// Returns the last occurrence of a value.
template <typename T> constexpr const T *find_last(const Range auto &p_range, const T &p_val) {
	for (size_t i = p_range.size(); i-- > 0;)
		if (p_range[i] == p_val)
			return { &p_range[i] };
	return {};
}

// Finds the first element that satisfies a given predicate.
template <typename T, typename P> constexpr const T *find_if(const Range auto &p_range, P p_pred) {
	for (size_t i = 0; i < p_range.size(); i++)
		if (p_pred(p_range[i]))
			return { &p_range[i] };

	return {};
}

// Finds the first element that satisfies a given predicate.
template <typename T, typename P> constexpr const T *find_if_not(const Range auto &p_range, P p_pred) {
	for (size_t i = 0; i < p_range.size(); i++)
		if (!p_pred(p_range[i]))
			return { &p_range[i] };

	return {};
}

// Checks if all elements in a range satisfy a predicate.
template <typename T, typename P> constexpr bool all_of(const Range auto &p_range, P p_pred) { return find_if_not<T, P>(p_range, p_pred) == nullptr; }

// Checks if any element in a range satisfies a predicate.
template <typename T, typename P> constexpr bool any_of(const Range auto &p_range, P p_pred) { return find_if<T, P>(p_range, p_pred) != nullptr; }

// Checks if no elements in a range satisfy a predicate.
template <typename T, typename P> constexpr bool none_of(const Range auto &p_range, P p_pred) { return find_if<T, P>(p_range, p_pred) == nullptr; }

} // namespace lt
