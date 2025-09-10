#pragma once

#include "lt/types/conditional.hpp"

struct EmptyType {};
struct Debug {};

// Type that is only enabled in debug mode. If debug mode is off then it will have 0 size (uses 1 byte) and can use EBO.
#define DebugType(...) lt::conditional_t<DEBUG_ENABLED, __VA_ARGS__, EmptyType>

#if defined(_MSC_VER)
#define at_no_unique [[msvc::no_unique_address]]
#else
#define at_no_unique [[no_unique_address]]
#endif

// Use DebugProperty on a non-static class member that should have 0 size (uses 0 bytes) outside of debug mode. Good for avoiding ifdefs all over.
#define DebugProperty at_no_unique DebugType

// Use to define debug only functions without ifdefs all over.
// The 'requires' clause can only be used in a template function so creates a dummy template parameter that does nothing so the function can use concepts.
#define debug_func(m_sig) template <typename T = Debug> m_sig requires(DEBUG_ENABLED == 1)
#define debug_impl template <>
