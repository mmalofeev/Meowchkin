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
    static bool first_call = true;
    static thread_local std::mt19937 rd_generator;
    if (first_call) {
        std::mt19937::result_type s =
            rd_generator() ^
            ((std::mt19937::result_type)std::chrono::duration_cast<std::chrono::seconds>(
                 std::chrono::system_clock::now().time_since_epoch()
             )
                 .count() +
             (std::mt19937::result_type)std::chrono::duration_cast<std::chrono::microseconds>(
                 std::chrono::high_resolution_clock::now().time_since_epoch()
             )
                 .count());
        rd_generator.seed(s);
        first_call = false;
    }
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
