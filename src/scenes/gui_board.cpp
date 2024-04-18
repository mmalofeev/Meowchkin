#include "gui_board.hpp"
#include "message_types.hpp"
#include "paths_to_binaries.hpp"

namespace meow {

void GuiBoard::setup(raylib::Window *window, GuiCardSpan *hand, network::Client *client) {
    if (window == nullptr || hand == nullptr || client == nullptr) {
        throw std::invalid_argument("invalid pointer setted in gui board!");
    }
    m_player_hand = hand;
    m_window = window;
    m_client = client;
    const raylib::Vector2 offset = {(m_window->GetWidth() - width) / 2.0f, 10.0f};
    m_rect = raylib::Rectangle(offset.x, offset.y, width, height);
    const float rect_side = 10;
    m_drop_card_rect = raylib::Rectangle(
        offset.x + (m_rect.width - rect_side) / 2, offset.y + (m_rect.height - rect_side) / 2,
        rect_side, rect_side
    );
    raylib::Image image;
    try {
        image.Load(board_image_path);
        image.Resize(m_rect.GetSize().x, m_rect.GetSize().y);
    } catch (const raylib::RaylibException &) {
        image =
            raylib::Image::Color(m_rect.GetSize().x, m_rect.GetSize().y, raylib::Color::RayWhite());
    }
    m_texture.Load(image);
    m_active_cards.set_span_borders(m_rect, {m_rect.x, 0});
}

void GuiBoard::draw(float frame_time, bool is_pause) {
#ifdef MANDELBROTSET
    float t = m_window->GetTime();
    static float zoom = 1;
    if (zoom < 1) {
        zoom = 1;
    }
    static raylib::Vector2 poi(0, 0);
    if (m_rect.CheckCollision(raylib::Mouse::GetPosition())) {
        if (raylib::Mouse::IsButtonDown(MOUSE_BUTTON_LEFT) &&
            !m_player_hand->selected().has_value()) {
            poi += raylib::Mouse::GetDelta() / (raylib::Vector2(1920, 1080) * zoom);
        }
        zoom += raylib::Mouse::GetWheelMove();
    }
    m_shader.BeginMode();
    m_shader.SetValue(m_shader.GetLocation("u_texture"), m_texture);
    m_shader.SetValue(m_shader.GetLocation("time"), &t, SHADER_UNIFORM_FLOAT);
    m_shader.SetValue(m_shader.GetLocation("zoom"), &zoom, SHADER_UNIFORM_FLOAT);
    m_shader.SetValue(m_shader.GetLocation("poi"), &poi, SHADER_UNIFORM_VEC2);
#endif
    m_texture.Draw(m_rect.GetPosition(), raylib::Color(0xFFFFFF44));
#ifdef MANDELBROTSET
    m_shader.EndMode();
#endif
    m_rect.DrawRoundedLines(0.1, 4, 5, raylib::Color::RayWhite());
    m_drop_card_rect.DrawLines(raylib::Color::Green());
    if (m_player_hand->selected().has_value() &&
        m_drop_card_rect.CheckCollision(m_player_hand->selected().value()->border)) {
        // m_active_cards.add_card(m_player_hand->pop_selected());
        add_card(m_player_hand->pop_selected().filename);
    }
    m_active_cards.draw_cards(frame_time, is_pause);
}

void GuiBoard::add_card(std::string_view card_filename) {
    // m_active_cards.add_card(card_filename);
    for (auto &info : m_client->get_players_info()) {
        std::cout << "send to " << info.name << '\n';
        m_client->send_action(
            network::Action(card_filename.data(), info.id, m_client->get_id_of_client())
        );
    }
}

}  // namespace meow
