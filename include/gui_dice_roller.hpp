#ifndef GUI_DICE_ROLLER_HPP_
#define GUI_DICE_ROLLER_HPP_

#include <chrono>
#include <concepts>
#include <limits>
#include <random>
#include "raylib-cpp.hpp"

namespace meow {

using namespace std::chrono_literals;

template <typename T = int>
    requires std::is_integral<T>::value
[[nodiscard]] T
random_integer(T low = std::numeric_limits<T>::min(), T high = std::numeric_limits<T>::max()) {
    static thread_local std::mt19937 rd_generator;
    return std::uniform_int_distribution<T>(low, high)(rd_generator);
}

[[nodiscard]] int play_dice_animation(
    raylib::Window *window,
    int low = 1,
    int high = 6,
    std::chrono::duration<long double> animation_time = 1.0s
) {
    // static_assert(false);
    if (window == nullptr) {
        throw std::runtime_error("invalid window pointer!");
    }

    int result = random_integer(low, high);
    for (auto start = std::chrono::steady_clock::now();
         std::chrono::steady_clock::now() < start + animation_time;) {
        result = random_integer(low, high);
    }
    return result;
}

}  // namespace meow

#endif  // GUI_DICE_ROLLER_HPP_
