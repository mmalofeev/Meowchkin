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

#gtest
enable_testing()
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/release-1.12.0.zip
)
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

#SQLite
#find_package(SQLite3 QUIET)
#if (NOT SQLITE3_FOUND)
#    include(FetchContent)
#    FetchContent_Declare(
#            sqlite3
#            URL https://www.sqlite.org/2023/sqlite-amalgamation-3410100.zip
#    )
#    FetchContent_MakeAvailable(sqlite3)
#endif ()

add_executable(tests src/scenes/scene.cpp tests/test_model.cpp src/shared_game_state.cpp src/model_card.cpp src/model_card_manager.cpp src/model_player.cpp src/virtual_machine.cpp  src/game_state.cpp)
target_link_libraries(tests gtest gtest_main ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} raylib raylib_cpp)


add_library(mainmenu-scene SHARED src/scenes/main_menu_scene.cpp src/scenes/scene.cpp)
target_link_libraries(mainmenu-scene ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} raylib raylib_cpp)

FILE(GLOB BEBRA src/*.cpp src/scenes/*.cpp)
add_library(gameview-scene SHARED
                src/scenes/raylib_game_view.cpp
                src/scenes/scene.cpp
                src/scenes/gui_card_span.cpp
                src/scenes/gui_board.cpp
                src/scenes/gui_usernames_box.cpp
                src/scenes/gui_text_chat.cpp
                src/client.cpp
                src/server.cpp
                src/model_card_manager.cpp
                src/model_card.cpp
                src/game_state.cpp
                src/shared_game_state.cpp
                src/virtual_machine.cpp)
target_link_libraries(gameview-scene ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} raylib raylib_cpp)

FILE(GLOB SRCFILES src/*.cpp src/scenes/scene.cpp)
add_executable(meowchkin ${SRCFILES})
target_link_libraries(meowchkin ${Boost_LIBRARIES} ${CMAKE_DL_LIBS} raylib raylib_cpp)