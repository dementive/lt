#pragma once

namespace lt {

namespace detail {

template <bool> struct condition;

template <> struct condition<false> {
	template <typename True, typename False> using type = False;
};

template <> struct condition<true> {
	template <typename True, typename False> using type = True;
};

} // namespace detail

// Faster (compile time) alternative to std::conditional
template <bool Condition, typename True, typename False> using conditional_t = typename detail::condition<Condition>::template type<True, False>;

} // namespace lt
