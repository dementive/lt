cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(MyCppProject VERSION 1.0.0 LANGUAGES CXX)

#set(CMAKE_CXX_COMPILER "clang++")
# set(CMAKE_CXX_COMPILER "g++")

# set(CMAKE_SYSTEM_NAME Windows)

# set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)
# set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
# set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
# set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)
# set(CMAKE_FIND_ROOT_PATH /usr/${TOOLCHAIN_PREFIX})

# set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
# set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_BUILD_TYPE Debug)

add_compile_options(-Wall -Wextra -Wpedantic -Wshadow)

#add_compile_options(-fsanitize=address)
#add_link_options(-fsanitize=address)

#add_compile_options(-fsanitize=undefined)
#add_link_options(-fsanitize=undefined)

# add_compile_options(-fsanitize=thread)
# add_link_options(-fsanitize=thread)

# add_compile_options(-fsanitize=memory)
# add_link_options(-fsanitize=memory)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_library(lt_library INTERFACE)

target_include_directories(lt_library
    INTERFACE ${CMAKE_SOURCE_DIR}/src
)
add_executable(test "test/main.cpp")
target_sources(test PRIVATE "src/lt/defs/stb_sprintf.cpp")
target_sources(test PRIVATE "src/lt/defs/log.cpp")

target_link_libraries(test PUBLIC lt_library)
