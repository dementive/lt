#pragma once

namespace lt::detail {

template <typename T> struct remove_reference {
	using type = T;
};

template <typename T> struct remove_reference<T &> {
	using type = T;
};

template <typename T> struct remove_reference<T &&> {
	using type = T;
};

template <typename T> using remove_reference_t = typename remove_reference<T>::type;

} // namespace lt::detail

// Faster (compile time) alternatives to std::move / std::forward. From: https://www.foonathan.net/2020/09/move-forward/
#define LT_MOV(...) static_cast<lt::detail::remove_reference_t<decltype(__VA_ARGS__)> &&>(__VA_ARGS__)
#define LT_FWD(...) static_cast<decltype(__VA_ARGS__) &&>(__VA_ARGS__)
