cmake_minimum_required(VERSION 3.10)

project(meowchkin)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g -fsanitize=address -fsanitize=undefined")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O2")

include_directories("include/"
        "third_party/")

FILE(GLOB SRCFILES src/*.cpp include/*.h)

include(FetchContent)

FetchContent_Declare(
    raylib_external
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
)

FetchContent_MakeAvailable(raylib_external)

add_executable(meowchkin ${SRCFILES})

target_link_libraries(meowchkin raylib)
