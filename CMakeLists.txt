cmake_minimum_required(VERSION 3.10)

project(meowchkin)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(suppressed_warnings "-Wno-sign-compare -Wno-unused-function -Wno-unused-parameter -Wno-missing-field-initializers") # sorry
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS ${suppressed_warnings})
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g -fsanitize=address -fsanitize=undefined -fno-gnu-unique")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O2")

include_directories("include/"
        "third_party/")

# boost
set(CMAKE_CXX_VISIBILITY_PRESET hidden)  # Recommended by Boost.DLL's manual
find_package(Boost 1.71 REQUIRED filesystem)  # Required by Boost.DLL
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

# raylib
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG 5.0
            GIT_SHALLOW 1
    )
    FetchContent_MakeAvailable(raylib)
endif ()

# raylib-cpp
find_package(raylib_cpp QUIET)
if (NOT raylib_cpp_FOUND)
    include(FetchContent)

    FetchContent_Declare(
            raylib_cpp
            GIT_REPOSITORY https://github.com/RobLoach/raylib-cpp.git
            GIT_TAG v5.0.1
    )
    FetchContent_MakeAvailable(raylib_cpp)
endif ()

add_library(mainmenu-scene SHARED src/scenes/main_menu_scene.cpp src/scenes/scene.cpp)
target_link_libraries(mainmenu-scene ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} raylib raylib_cpp)

add_library(game-scene SHARED src/scenes/game_view.cpp
                              src/scenes/scene.cpp
                              src/scenes/gui_card_span.cpp
                              src/scenes/gui_board.cpp
                              src/scenes/gui_usernames_box.cpp
                              src/scenes/gui_text_chat.cpp
                              src/client.cpp
                              src/server.cpp)
target_link_libraries(game-scene ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} raylib raylib_cpp)

FILE(GLOB SRCFILES src/*.cpp src/scenes/scene.cpp)
add_executable(meowchkin ${SRCFILES})
target_link_libraries(meowchkin ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} raylib raylib_cpp)