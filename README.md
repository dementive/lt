# lt

Lightweight templates for C++ 23.

## Containers

| Container                         | Description                                                                                 |
|-----------------------------------|---------------------------------------------------------------------------------------------|
| **[array](src/array.hpp)**                | Wrapper around a C array                                                                    |
| **[const_map](src/const_map.hpp)**        | Const compile time map                                                                      |
| **[fixed_map](src/fixed_map.hpp)**        | Fixed size compile time map                                                                 |
| **[fixed_set](src/fixed_set.hpp)**        | Fixed size compile time set                                                                 |
| **[fixed_string](src/fixed_string.hpp)**  | Fixed size string                                                                           |
| **[fixed_vector](src/fixed_vector.hpp)**  | Fixed size vector                                                                           |
| **[list](src/list.hpp)**                  | Intrusive doubly linked list                                                                |
| **[pair](src/pair.hpp)**                  | Basic version of std::pair                                                                  |
| **[range](src/range.hpp)**                | Modifiable view into a contiguous memory space                                              |
| **[span](src/span.hpp)**                  | Const view into a contiguous memory space.                                                  |
| **[tuple](src/tuple.hpp)**                | Basic version of std::tuple                                                                 |


| Types                                 | Description                                                                                 |
|---------------------------------------|---------------------------------------------------------------------------------------------|
| **[defer](src/types/defer.hpp)**                    | Call a callback on scope exit.                                                              |
| **[either](src/types/either.hpp)**                  | A type that is either one type or another type                                              |
| **[option](src/types/option.hpp)**                  | Basic version of std::optional                                                              |
| **[result](src/types/result.hpp)**                  | Basic version of std::expected                                                              |

## Features

- No dynamic memory allocation

- No exceptions

- No rtti

- Everything is constexpr

- The only standard library headers used are:  `<new>`, `<stddef.h>`, and `<initializer_list>`.
