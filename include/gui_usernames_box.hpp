#ifndef GUI_USER_NAMES_BOX_
#define GUI_USER_NAMES_BOX_

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include "gui_window_dependable.hpp"
#include "raylib-cpp.hpp"

namespace meow {

class GuiUsernamesBox : public WindowDependable<GuiUsernamesBox> {
private:
    static constexpr int width = 250;
    static constexpr int single_height = 50;
    static constexpr int padding = 5;

    std::unordered_map<std::size_t, std::string> m_usernames;
    std::vector<raylib::Rectangle> m_boxes;
    raylib::Rectangle m_border;
    raylib::Window *m_window = nullptr;

public:
    GuiUsernamesBox() = default;
    std::size_t active_user = 0;

    void add_username(std::pair<std::size_t, std::string_view> user_info);
    void draw(std::size_t current_user_turn);
    void set_window(raylib::Window *window);
};

}  // namespace meow

#endif  // GUI_USER_NAMES_BOX_
