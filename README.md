# lt

Lightweight templates for C++ 23.

## Containers

| Container                         | Description                                                                                 |
|-----------------------------------|---------------------------------------------------------------------------------------------|
| **[array](array)**                | Wrapper around a C array                                                                    |
| **[const_map](const_map)**        | Const compile time map                                                                      |
| **[fixed_map](fixed_map)**        | Fixed size compile time map                                                                 |
| **[fixed_set](fixed_set)**        | Fixed size compile time set                                                                 |
| **[fixed_string](fixed_string)**  | Fixed size string                                                                           |
| **[fixed_vector](fixed_vector)**  | Fixed size vector                                                                           |
| **[list](list)**                  | Intrusive doubly linked list                                                                |
| **[pair](pair)**                  | Basic version of std::pair                                                                  |
| **[range](range)**                | Modifiable view into a contiguous memory space                                              |
| **[span](span)**                  | Const view into a contiguous memory space.                                                  |
| **[tuple](tuple)**                | Basic version of std::tuple                                                                 |


| Types                                 | Description                                                                                 |
|---------------------------------------|---------------------------------------------------------------------------------------------|
| **[defer](defer)**                    | Call a callback on scope exit.                                                              |
| **[either](either)**                  | A type that is either one type or another type                                              |
| **[option](option)**                  | Basic version of std::optional                                                              |
| **[result](result)**                  | Basic version of std::expected                                                              |

## Features

- No dynamic memory allocation

- No exceptions

- No rtti

- Everything is constexpr

- The only standard library headers used are:  `<new>`, `<stddef.h>`, and `<initializer_list>`.
