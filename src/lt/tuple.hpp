#pragma once

#include "defs/move.hpp"

namespace lt {

template <typename... Types> struct tuple;

template <> struct tuple<> {};

template <typename T, typename... Args> struct tuple<T, Args...> : tuple<Args...> {
	T value;

	constexpr tuple() = default;

	template <typename F, typename... R>
	constexpr tuple(F &&f, R &&...rest) :
			tuple<Args...>(LT_FWD(rest)...),
			value(LT_FWD(f)) {}
};

namespace detail {
template <int I, typename T> struct tuple_get_t;
template <typename F, typename... Args> struct tuple_get_t<0, tuple<F, Args...>> {
	static constexpr F &get(tuple<F, Args...> &t) { return t.value; }
};

template <int I, typename F, typename... Args> struct tuple_get_t<I, tuple<F, Args...>> {
	static constexpr auto &get(tuple<F, Args...> &t) { return tuple_get_t<I - 1, tuple<Args...>>::get(static_cast<tuple<Args...> &>(t)); }
};
} // namespace detail

template <int I, typename... Types> constexpr auto &get(tuple<Types...> &t) { return detail::tuple_get_t<I, tuple<Types...>>::get(t); }
template <int I, typename... Types> constexpr const auto &get(const tuple<Types...> &t) { return detail::tuple_get_t<I, tuple<Types...>>::get(t); }

} // namespace lt
