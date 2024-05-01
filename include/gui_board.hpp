#ifndef GUI_BOARD_HPP
#define GUI_BOARD_HPP

#include "client.hpp"
#include "gui_card_span.hpp"
#include "gui_card_span_dropdown_menu.hpp"
#include "raylib-cpp.hpp"

namespace meow {

class GuiBoard {
    friend class RaylibGameView;

private:
    static constexpr const int width = 1200;
    static constexpr const int height = 700;
    GuiCardSpan *m_player_hand = nullptr;
    raylib::Window *m_window = nullptr;
    network::Client *m_client = nullptr;
    raylib::Rectangle m_rect;
    raylib::Texture m_texture;
    raylib::Rectangle m_drop_card_rect;
    GuiCardSpan m_active_cards;
#ifdef MANDELBROTSET
    raylib::Shader m_shader = raylib::Shader(nullptr, "bin/shaders/mandelbrot-set.fs");
#endif

public:
    GuiBoard() : m_active_cards(std::make_unique<DropDownMenu>(&m_active_cards)) {
    }

    void setup(raylib::Window *window, GuiCardSpan *hand, network::Client *client);
    void draw(float frame_time, bool is_pause);
    void add_card(std::string_view card_filename);

    void remove_card(std::string_view card_filename) {
        m_active_cards.remove_card(card_filename);
    }
};

}  // namespace meow

#endif  // GUI_BOARD_HPP
