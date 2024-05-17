#ifndef MODEL_RANDOM_HPP_
#define MODEL_RANDOM_HPP_
#include <concepts>
#include <limits>
#include <random>
#include "model_object.hpp"

namespace meow::model {

// взял Пашин код для модели.
template <typename T = int>
    requires std::is_integral<T>::value
[[nodiscard]] T get_object_based_random_integer(
    T low = std::numeric_limits<T>::min(),
    T high = std::numeric_limits<T>::max()
) {
    static thread_local bool first_call = true;
    static thread_local std::mt19937 rd_generator;
    if (first_call) {
        rd_generator.seed(Object().obj_id);
        first_call = false;
    }
    return std::uniform_int_distribution<T>(low, high)(rd_generator);
}

}  // namespace meow::model

#endif