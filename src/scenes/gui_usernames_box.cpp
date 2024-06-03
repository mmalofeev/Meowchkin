#include "gui_usernames_box.hpp"
#include "Mouse.hpp"
#include "paths_to_binaries.hpp"
#include "raylib.h"

void meow::GuiUsernamesBox::add_username(std::pair<std::size_t, std::string_view> user_info) {
    m_usernames.emplace(user_info);
    m_boxes.emplace_back(
        m_border.x, m_border.height + padding * (m_boxes.size() + 1), width, single_height
    );
    m_border.height += single_height;
}

void meow::GuiUsernamesBox::draw(std::size_t current_user_turn) {
    static const raylib::Color colors[] = {0x2F3C7EAA, 0x3B3FEEAA, 0x2F3C7EAA, 0xFBEAEBAA};
    static const raylib::Font font = raylib::LoadFont(gui_font_path);

    auto it = m_usernames.begin();
    for (const auto &box : m_boxes) {
        box.DrawGradientH(colors[0], colors[1]);
        raylib::Color c =
            active_user == it->first ? raylib::Color::Green() : raylib::Color::Red();
        box.DrawLines(c, 1);
        if (box.CheckCollision(raylib::Mouse::GetPosition()) &&
            raylib::Mouse::IsButtonPressed(MOUSE_BUTTON_LEFT)) {
            active_user = it->first;
        }
        raylib::DrawTextEx(font, it++->second, {box.x, box.y}, 16, 1, raylib::Color::RayWhite());
    }

    raylib::Rectangle(
        m_boxes.begin()->GetPosition(),
        m_boxes.back().GetPosition() + m_boxes.back().GetSize() - raylib::Vector2(0, padding)
    )
        .DrawRoundedLines(0.03, 4, 2, raylib::Color::Red());
}

void meow::GuiUsernamesBox::set_window(raylib::Window *window) {
    if (window == nullptr) {
        throw std::runtime_error("invalid window ptr!");
    }
    m_window = window;
    m_border.x = window->GetWidth() - width;
    m_border.y = 0;
    m_border.width = width;
    m_border.height = 0;
}
