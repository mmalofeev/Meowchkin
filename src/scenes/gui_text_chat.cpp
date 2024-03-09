#include "gui_text_chat.hpp"
#include <cstring>
#include <algorithm>
#include "raygui.h"

void meow::GuiTextChat::set_window(raylib::Window *window) {
    m_window = window;
    m_border.x = window->GetWidth() - (message_rectangle_size.x + 10);
    m_border.y = 400;
    m_border.width = message_rectangle_size.x;
    m_border.height = 0;
    position = m_border.GetPosition();
}

void meow::GuiTextChat::receive(const Message &msg) {
    m_messages.push_back(msg);
    if (m_messages.size() > max_messages) {
        m_messages.pop_front();
        m_border.height -= message_rectangle_size.y;
    }
    m_border.height += message_rectangle_size.y;
}

void meow::GuiTextChat::draw() {
    m_border.SetPosition(position);
    raylib::Vector2 offset{0, 0};
    for (const auto &msg : m_messages) {
        raylib::Rectangle(position + offset, message_rectangle_size).Draw(raylib::Color::SkyBlue());
        raylib::DrawText(
            msg.sender + ": " + msg.text, (position + offset).x, (position + offset).y, 20,
            raylib::Color::Blue()
        );
        offset.y += message_rectangle_size.y;
    }
    if (GuiTextBox(
            raylib::Rectangle(
                m_border.GetPosition() + m_border.GetSize() +
                    raylib::Vector2(-message_rectangle_size.x, 0),
                message_rectangle_size
            ),
            msg, 100, true
        ) &&
        std::strlen(msg) > 0) {
        for (std::size_t i = 0, len = std::strlen(msg); i < len; i += 20) {
            receive({std::string(msg + i, msg + std::min(i + 20, len)), "sender"});
        }
        msg[0] = 0;
    }
    m_border.DrawLines(raylib::Color::Red());
}
