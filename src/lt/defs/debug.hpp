#pragma once

#include "lt/types/conditional.hpp"

struct EmptyType {};
struct Debug {};

// Type that is only enabled in debug mode. If debug mode is off then it will have 0 size (uses 1 byte) and can use EBO.
#define DebugType(...) lt::conditional_t<DEBUG_ENABLED, __VA_ARGS__, EmptyType>

// https://brevzin.github.io/c++/2021/11/21/conditional-members/#conditional-member-variables
// https://en.cppreference.com/w/cpp/language/attributes/no_unique_address
#if defined(_MSC_VER)
#define LT_NO_UNIQUE [[msvc::no_unique_address]]
#else
#define LT_NO_UNIQUE [[no_unique_address]]
#endif

// Use DebugProperty on a non-static class member that should have 0 size (uses 0 bytes) outside of debug mode. Good for avoiding ifdefs all over.
#define DebugProperty LT_NO_UNIQUE DebugType

// Use to define debug only functions without ifdefs all over.
// The 'requires' clause can only be used in a template function so creates a dummy template parameter that does nothing so the function can use concepts.
#define debug_func(m_sig) template <typename T = Debug> m_sig requires(DEBUG_ENABLED == 1)
#define debug_impl template <>
