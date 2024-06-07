#ifndef GUI_CARD_HPP_
#define GUI_CARD_HPP_

#include "raylib-cpp.hpp"

#define dbg                                                     \
    do {                                                        \
        std::cout << __FILE__ << ": " << __LINE__ << std::endl; \
    } while (0)

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
    std::size_t card_id = -1;  // hehe
    // bool valid_target = false;
};

// not for display
struct GuiCardInfo {
    raylib::Rectangle intersect;
    std::size_t card_id;
};

}  // namespace meow

#endif  // GUI_CARD_HPP_
