#pragma once

#include "lt/defs/assert.hpp"
#include "lt/types/concepts.hpp"

#include <stddef.h>

namespace lt {

// Const view into a contiguous memory space.
template <typename T> struct span {
	constexpr span() = default;
	constexpr span(const T *p_ptr, size_t plength) :
			ptr(p_ptr),
			length(plength) {}

	template <size_t N>
	constexpr span(const T (&p_array)[N]) :
			ptr(p_array),
			length(N) {}

	constexpr span(lt::detail::Spanable auto &x) :
			ptr(x.begin()),
			length(x.size()) {}

	span &operator=(const span &) = delete;
	span &operator=(span &&) = delete;

	constexpr size_t size() const { return length; }
	constexpr bool is_empty() const { return length == 0; }

	constexpr const T &operator[](size_t p_idx) const { return ptr[p_idx]; }
	constexpr const T &at(size_t p_idx) const {
		lt_assert_idx(p_idx < length);
		return ptr[p_idx];
	}

	constexpr const T *begin() const { return ptr; }
	constexpr const T *end() const { return ptr + length; }

	constexpr span<T> subspan(size_t offset, size_t count) const {
		lt_assert_msg(offset <= length || offset + count <= length, "subspan construction out of bounds");
		return { ptr + offset, count };
	}

	constexpr span<T> first(int count) const { return subspan(0, count); }
	constexpr span<T> last(int count) const { return subspan(length - count, count); }

	const T *ptr = nullptr;

private:
	size_t length = 0;
};

template <typename T> constexpr span<T> to_span(lt::detail::Spanable auto &x) { return { x.begin(), x.size() }; }

} // namespace lt
