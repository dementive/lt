// Search and query algorithms
#pragma once

#include "lt/span.hpp"

namespace lt {

// Returns the first occurrence of a value, searching from the beginning.
template <typename T> constexpr const T *find(const span<T> &p_span, const T &p_val) {
	for (size_t i = 0; i < p_span.size(); i++)
		if (p_span.ptr[i] == p_val)
			return { &p_span.ptr[i] };
	return {};
}

// Returns the last occurrence of a value.
template <typename T> constexpr const T *find_last(const span<T> &p_span, const T &p_val) {
	for (size_t i = p_span.size(); i-- > 0;)
		if (p_span.ptr[i] == p_val)
			return { &p_span.ptr[i] };
	return {};
}

// Finds the first element that satisfies a given predicate.
template <typename T, typename P> constexpr const T *find_if(const span<T> &p_span, P p_pred) {
	for (size_t i = 0; i < p_span.size(); i++)
		if (p_pred(p_span.ptr[i]))
			return { &p_span.ptr[i] };

	return {};
}

// Finds the first element that satisfies a given predicate.
template <typename T, typename P> constexpr const T *find_if_not(const span<T> &p_span, P p_pred) {
	for (size_t i = 0; i < p_span.size(); i++)
		if (!p_pred(p_span.ptr[i]))
			return { &p_span.ptr[i] };

	return {};
}

// Checks if all elements in the span satisfy a predicate.
template <typename T, typename P> constexpr bool all_of(const span<T> &p_span, P p_pred) { return find_if_not(p_span, p_pred) == nullptr; }

// Checks if any element in the span satisfies a predicate.
template <typename T, typename P> constexpr bool any_of(const span<T> &p_span, P p_pred) { return find_if(p_span, p_pred) != nullptr; }

// Checks if no elements in the span satisfy a predicate.
template <typename T, typename P> constexpr bool none_of(const span<T> &p_span, P p_pred) { return find_if(p_span, p_pred) == nullptr; }

} // namespace lt
