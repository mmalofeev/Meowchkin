#ifndef GUI_TEXT_CHAT_HPP
#define GUI_TEXT_CHAT_HPP

#include <deque>
#include <string>
#include "client.hpp"
#include "gui_window_dependable.hpp"
#include "raylib-cpp.hpp"

namespace meow {

class GuiTextChat : WindowDependable<GuiTextChat> {
private:
    inline static const raylib::Vector2 message_rectangle_size{325, 25};
    static constexpr int max_messages = 13;
    static constexpr std::size_t max_message_length = 100;
    raylib::Window *m_window;
    raylib::Rectangle m_border;
    std::deque<network::ChatMessage> m_messages;
    char msg[max_message_length];

public:
    // inits on window attach!
    raylib::Vector2 position = {};  // NOLINT

    [[nodiscard]] const raylib::Rectangle &get_border() const {
        return m_border;
    }

    void receive(const network::ChatMessage &msg);
    void draw(network::Client &client);
    void set_window(raylib::Window *window);
};

}  // namespace meow

#endif  // GUI_TEXT_CHAT_HPP
