#pragma once

#include "lt/defs/crash.hpp"
#include "lt/types/concepts.hpp"

#include <stddef.h>

namespace lt {

// Modifiable view into a contiguous memory space.
template <typename T> struct range {
	constexpr range() = default;
	constexpr range(T *p_ptr, size_t plength) :
			ptr(p_ptr),
			length(plength) {}

	constexpr range(lt::detail::Spanable auto &x) :
			ptr(x.begin()),
			length(x.size()) {}

	constexpr size_t size() const { return length; }
	constexpr bool is_empty() const { return length == 0; }

	constexpr T &operator[](size_t p_idx) const { return ptr[p_idx]; }
	constexpr T &at(size_t p_idx) const {
		if (p_idx >= length)
			lt_crash("index out of bounds");
		return ptr[p_idx];
	}

	constexpr T *begin() const { return ptr; }
	constexpr T *end() const { return ptr + length; }

	constexpr range<T> subrange(int offset, int count) const {
		if (offset > length || offset + count > length)
			lt_crash("subrange construction out of bounds");
		return { ptr + offset, count };
	}

	constexpr range<T> first(int count) const { return subrange(0, count); }
	constexpr range<T> last(int count) const { return subrange(length - count, count); }

	T *ptr = nullptr;

private:
	size_t length = 0;
};

} // namespace lt
