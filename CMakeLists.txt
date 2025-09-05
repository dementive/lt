cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(MyCppProject VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_library(lt_library INTERFACE)

target_include_directories(lt_library
    INTERFACE ${CMAKE_SOURCE_DIR}/src
)
add_executable(test "test/main.cpp")
target_link_libraries(test PUBLIC lt_library)
