#ifndef GUI_CARD_HPP_
#define GUI_CARD_HPP_

#include "raylib-cpp.hpp"

namespace meow {

struct GuiCard {
    static constexpr long double ratio = 23.0 / 35;
    static constexpr const int height = 300;
    static constexpr const int width = height * ratio;
    raylib::Rectangle border;
    raylib::Vector2 target_position;
    raylib::Texture texture;
    raylib::Image orig_img;
    std::string filename;
    std::size_t card_id = -1;
};

}  // namespace meow

#endif  // GUI_CARD_HPP_
