#ifndef GUI_CARD_HPP
#define GUI_CARD_HPP

#include "raylib-cpp.hpp"

namespace meow {
struct GuiCard {
    static constexpr const int width = 230;
    static constexpr const int height = 350;
    raylib::Rectangle border;
    raylib::Vector2 target_position;
    raylib::Texture texture;
    // raylib::Rectangle full_sized_border;
    // raylib::Texture full_sized_texture;
};
}  // namespace meow

#endif  // GUI_CARD_HPP_
