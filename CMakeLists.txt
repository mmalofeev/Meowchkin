cmake_minimum_required(VERSION 3.10)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(suppressed_warnings "-Wno-sign-compare -Wno-unused-function -Wno-unused-parameter -Wno-missing-field-initializers ") # sorry
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS ${suppressed_warnings})
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g -fsanitize=address -fsanitize=undefined")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O2")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

project(meowchkin)

include_directories("include/"
        "third_party/")

FILE(GLOB SRCFILES src/*.cpp src/scenes/scene.cpp third_party/sqlite/sqlite3.c)

add_executable(bdusage src/BD_1_usage.cpp third_party/sqlite/sqlite3.c)
