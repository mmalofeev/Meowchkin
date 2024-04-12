#include "gui_text_chat.hpp"
#include <algorithm>
#include <cstring>
#include <map>
#include "raygui.h"

namespace {

std::string get_client_name(const meow::network::Client &client, std::size_t id) {
    static std::map<std::size_t, std::string> cache;
    if (auto it = cache.find(id); it != cache.end()) {
        return it->second;
    }
    auto info = client.get_players_info();
    return cache[id] = std::find_if(info.begin(), info.end(), [id](const auto &inf) {
                           return inf.id == id;
                       })->name;
}

}  // namespace

void meow::GuiTextChat::set_window(raylib::Window *window) {
    m_window = window;
    m_border.x = window->GetWidth() - (message_rectangle_size.x + 10);
    m_border.y = 400;
    m_border.width = message_rectangle_size.x;
    m_border.height = 0;
    position = m_border.GetPosition();
}

void meow::GuiTextChat::receive(const network::ChatMessage &msg) {
    m_messages.push_back(msg);
    if (m_messages.size() > max_messages) {
        m_messages.pop_front();
        m_border.height -= message_rectangle_size.y;
    }
    m_border.height += message_rectangle_size.y;
}

void meow::GuiTextChat::draw(network::Client &client) {
    m_border.SetPosition(position);
    raylib::Vector2 offset{0, 0};
    for (const auto &msg : m_messages) {
        raylib::Rectangle(position + offset, message_rectangle_size).Draw(raylib::Color::SkyBlue());
        std::string text = (std::stringstream{} << get_client_name(client, msg.sender_player)
                                                << ": " << msg.message)
                               .str();
        raylib::DrawText(
            text, (position + offset).x, (position + offset).y, 20, raylib::Color::Blue()
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
            // receive({std::string(msg + i, msg + std::min(i + 20, len)), "sender"});
            client.send_chat_message(network::ChatMessage(msg, client.get_id_of_client()));
        }
        msg[0] = 0;
    }
    m_border.DrawLines(raylib::Color::Red());
}
