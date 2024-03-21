#ifndef GUI_WINDOW_DEPENDABLE_HPP_
#define GUI_WINDOW_DEPENDABLE_HPP_

#include <stdexcept>
#include "raylib-cpp.hpp"

namespace meow {

template <typename Dependable>
struct WindowDependable {
    void set_window(raylib::Window *window) {
        if (window == nullptr) {
            throw std::invalid_argument("invalid window pointer setted!");
        }
        static_cast<Dependable *>(this)->set_window(window);
    }
};

}  // namespace meow

#endif  // GUI_WINDOW_DEPENDABLE_HPP_
