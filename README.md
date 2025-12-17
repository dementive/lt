# lt

Lightweight templates for C++ 23.

## Containers

| Container                         | Description                                                                                 |
|-----------------------------------|---------------------------------------------------------------------------------------------|
| **[array](src/lt/array.hpp)**                | Wrapper around a C array                                                                    |
| **[const_map](src/lt/const_map.hpp)**        | Const compile time map                                                                      |
| **[fixed_map](src/lt/fixed_map.hpp)**        | Fixed size compile time map                                                                 |
| **[fixed_set](src/lt/fixed_set.hpp)**        | Fixed size compile time set                                                                 |
| **[fixed_string](src/lt/fixed_string.hpp)**  | Fixed size string                                                                           |
| **[fixed_vector](src/lt/fixed_vector.hpp)**  | Fixed size vector                                                                           |
| **[list](src/lt/list.hpp)**                  | Intrusive doubly linked list                                                                |
| **[pair](src/lt/pair.hpp)**                  | Basic version of std::pair                                                                  |
| **[span](src/lt/span.hpp)**                  | Const view into a contiguous memory space.                                                  |
| **[tuple](src/lt/tuple.hpp)**                | Basic version of std::tuple                                                                 |


## Types
| Type                                  | Description                                                                                 |
|---------------------------------------|---------------------------------------------------------------------------------------------|
| **[defer](src/lt/types/defer.hpp)**                    | Call a callback on scope exit.                                             |
| **[either](src/lt/types/either.hpp)**                  | A type that is either one type or another type                             |
| **[option](src/lt/types/option.hpp)**                  | Basic version of std::optional                                             |
| **[result](src/lt/types/result.hpp)**                  | Basic version of std::expected                                             |


## Macros
| Macro                                  | Description                                                                                 |
|---------------------------------------|---------------------------------------------------------------------------------------------|
| **[DebugType](src/lt/defs/debug.hpp)**                    | Type that is only enabled in debug mode.                          |
| **[debug_func](src/lt/defs/debug.hpp)**                  | Function that is only enabled in debug mode.                  |
| **[FOR_EACH](src/lt/defs/for_each.hpp)**                  | Recursive macro.                  |
| **[test](src/lt/defs/test.hpp)**                  | Simple testing framework using recursive macros.                  |

## Features

- No dynamic memory allocation

- No exceptions

- No rtti

- Almost everything is constexpr

- The only standard library headers used are:  `<new>`, `<stddef.h>`, and `<initializer_list>`.

- Compiles cleanly with `-Wall -Wextra -Wpedantic -Wshadow` and all sanitizers.

## Credits

The tuple and list were adapted from the Tuple and SelfList from [Godot](https://github.com/godotengine/godot/tree/master/core/templates).

snprintf from [stb](https://github.com/nothings/stb/blob/master/stb_sprintf.h)
