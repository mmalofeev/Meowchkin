#include "gui_usernames_box.hpp"
#include "paths_to_binaries.hpp"

void meow::GuiUsernamesBox::add_username(std::string_view username) {
    m_usernames.push_back(username.data());
    m_boxes.emplace_back(
        m_border.x, m_border.height + padding * (m_boxes.size() + 1), width, single_height
    );
    m_border.height += single_height;
    // m_border.height = m_boxes.back().y + m_boxes.back().height - m_boxes.begin()->y;
}

void meow::GuiUsernamesBox::draw() const {
    std::size_t i = 0;
    static const raylib::Color colors[] = {0x2F3C7EAA, 0x3B3FEEAA, 0x2F3C7EAA, 0xFBEAEBAA};
    static const raylib::Font font = raylib::LoadFont(gui_font_path);
    for (const auto &box : m_boxes) {
        box.DrawGradientH(colors[0], colors[1]);
        box.DrawLines(raylib::Color::Green(), 1);
        raylib::DrawTextEx(
            font, m_usernames[i++], {box.x, box.y}, 16, 1, raylib::Color::RayWhite()
        );
    }
    // m_border.DrawRoundedLines(0.2, 3, 2, raylib::Color::Green());
    // m_border.DrawLines(raylib::Color::Green());
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
