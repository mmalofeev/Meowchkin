#ifndef GUI_BOARD_HPP
#define GUI_BOARD_HPP

#include "gui_card_span.hpp"
#include "raylib-cpp.hpp"

namespace meow {
class GuiBoard {
private:
    static constexpr const int width = 1200;
    static constexpr const int height = 700;
    static constexpr const char *image_path = "bin/imgs/kitik3.png";
    GuiCardSpan *m_player_hand = nullptr;
    raylib::Window *m_window = nullptr;
    raylib::Rectangle m_rect;
    raylib::Texture m_texture;
    raylib::Rectangle m_drop_card_rect;
    GuiCardSpan m_active_cards;
#ifdef MANDELBROTSET
    raylib::Shader m_shader = raylib::Shader(nullptr, "bin/shaders/mandelbrot-set.fs");
#endif

public:
    explicit GuiBoard() = default;

    void setup(raylib::Window *window, GuiCardSpan *hand);
    void draw(float frame_time, bool is_pause);
};
}  // namespace meow

#endif  // GUI_BOARD_HPP
