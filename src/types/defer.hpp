#pragma once

#include "lt/defs/move.hpp"

namespace lt {
namespace detail {

// defer calls a callback on scope exit.
template <typename F> struct defer_t {
	F func;
	defer_t(F f) :
			func(LT_MOV(f)) {}
	~defer_t() { func(); }
};

} // namespace detail

auto defer(auto &&f) { return detail::defer_t<decltype(f)>(static_cast<decltype(f) &&>(f)); }

} // namespace lt

#define lt_defer(m_code) lt::defer([&] { m_code })

// void test() {
//     void* p = malloc(9001);
//     auto _ = lt::defer([&] { free(p); });

// void* p2 = malloc(8999);
// auto _ = lt_defer(
// 	free(p2);
// );

// }
