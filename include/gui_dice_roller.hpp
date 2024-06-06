#ifndef GUI_DICE_ROLLER_HPP_
#define GUI_DICE_ROLLER_HPP_

#include <chrono>
#include <concepts>
#include <limits>
#include <random>
#include "raylib-cpp.hpp"

namespace meow {

using namespace std::chrono_literals;

template <std::integral T = int>
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

}  // namespace meow

#endif  // GUI_DICE_ROLLER_HPP_
