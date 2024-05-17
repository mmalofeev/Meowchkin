#ifndef GUI_TEXT_CHAT_HPP
#define GUI_TEXT_CHAT_HPP

#include <deque>
#include "client.hpp"
#include "gui_window_dependable.hpp"
#include "raylib-cpp.hpp"

namespace meow {

class GuiTextChat : WindowDependable<GuiTextChat> {
private:
    static constexpr int message_width = 1200;
    static constexpr int message_height = 25;
    static constexpr int max_messages = 700 / message_height - 1;
    static constexpr std::size_t max_message_length = 100;
    raylib::Window *m_window;
    raylib::Rectangle m_border;
    std::deque<network::ChatMessage> m_messages;
    char m_msg[max_message_length];

public:
    [[nodiscard]] const raylib::Rectangle &get_border() const {
        return m_border;
    }

    void receive(const network::ChatMessage &msg);
    void draw(network::Client &client);
    void set_window(raylib::Window *window);
};

}  // namespace meow

#endif  // GUI_TEXT_CHAT_HPP
