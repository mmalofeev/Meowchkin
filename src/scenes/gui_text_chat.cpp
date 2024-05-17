#include "gui_text_chat.hpp"
#include <algorithm>
#include <cstring>
#include <map>
#include "paths_to_binaries.hpp"
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
    const raylib::Vector2 offset = {(m_window->GetWidth() - message_width) / 2.0f, 70.0f};
    m_border.width = message_width;
    m_border.height = 0;
    m_border.SetPosition(offset);
}

void meow::GuiTextChat::receive(const network::ChatMessage &msg) {
    m_messages.push_back(msg);
    if (m_messages.size() > max_messages) {
        m_messages.pop_front();
        m_border.height -= message_height;
    }
    m_border.height += message_height;
}

void meow::GuiTextChat::draw(network::Client &client) {
    raylib::Vector2 offset = {(m_window->GetWidth() - message_width) / 2.0f, 70.0f};
    for (const auto &msg : m_messages) {
        static const raylib::Font font = raylib::LoadFontEx(gui_font_path, 20.0f, 0, 0);
        raylib::Rectangle(offset, raylib::Vector2{message_width, message_height})
            .Draw(raylib::Color(0xFFFFFF44));
        std::string text = (std::stringstream{} << get_client_name(client, msg.sender_player)
                                                << ": " << msg.message)
                               .str();
        raylib::DrawTextEx(
            font, text, {offset.x + 5.0f, offset.y + 5.0f}, 20.0f, 0.0f, raylib::Color::RayWhite()
        );
        offset.y += message_height;
    }

    if (GuiTextBox(
            raylib::Rectangle(
                m_border.GetPosition() + m_border.GetSize() + raylib::Vector2(-message_width, 0),
                raylib::Vector2(message_width, message_height)
            ),
            m_msg, 100, true
        ) &&
        std::strlen(m_msg) > 0) {
        client.send_chat_message(network::ChatMessage(m_msg, client.get_id_of_client()));
        m_msg[0] = 0;
    }
    m_border.DrawLines(raylib::Color::Red());
}
