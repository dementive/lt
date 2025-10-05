#pragma once

#include "lt/defs/for_each.hpp"
#include "lt/types/conditional.hpp"

struct EmptyType {};
struct Debug {};

// Type that is only enabled in debug mode. If debug mode is off then it will have 0 size (uses 1 byte) and can use EBO.
#define DebugType(...) lt::conditional_t<DEBUG_ENABLED, __VA_ARGS__, EmptyType>
#define ToolType(...) lt::conditional_t<TOOLS_ENABLED, __VA_ARGS__, EmptyType>

#if defined(_MSC_VER)
#define lt_no_unique [[msvc::no_unique_address]]
#else
#define lt_no_unique [[no_unique_address]]
#endif

// Use DebugProperty on a non-static class member that should have 0 size (uses 0 bytes) outside of debug mode. Good for avoiding ifdefs all over.
#define DebugProperty lt_no_unique DebugType
#define ToolProperty lt_no_unique ToolType

// Use to define debug only functions without ifdefs all over.
// The 'requires' clause can only be used in a template function so creates a dummy template parameter that does nothing so the function can use concepts.
#define debug_func(m_sig) template <typename T = Debug> m_sig requires(DEBUG_ENABLED == 1);
#define debug_impl template <>

#define tool_func(m_sig) template <typename T = Debug> m_sig requires(TOOLS_ENABLED == 1);
#define tool_impl template <>

#define debug_funcs(...) FOR_EACH(debug_func, __VA_OPT__(__VA_ARGS__, ))
#define tool_funcs(...) FOR_EACH(tool_func, __VA_OPT__(__VA_ARGS__, ))

#ifdef TOOLS_ENABLED
#define tool_code(...) __VA_ARGS__
#else
#define tool_code(...)
#endif

#ifdef DEBUG_ENABLED
#define debug_code(...) __VA_ARGS__
#else
#define debug_code(...)
#endif
