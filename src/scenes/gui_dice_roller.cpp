#include "gui_dice_roller.hpp"

namespace meow {
namespace {
thread_local std::mt19937 rd_generator;
}

template <typename T = int>
    requires std::is_integral<T>::value
T random_integer(T low = std::numeric_limits<T>::min(), T high = std::numeric_limits<T>::max()) {
    return std::uniform_int_distribution<T>(low, high)(rd_generator);
}
}  // namespace meow
