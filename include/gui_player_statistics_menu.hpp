#ifndef GUI_PLAYER_STATISTICS_MENU_HPP_
#define GUI_PLAYER_STATISTICS_MENU_HPP_

#include <string>
#include "enum_array.hpp"
#include "gui_board.hpp"
#include "raygui.h"
#include "raylib-cpp.hpp"

namespace meow {

struct GuiPlayerStatisticsMenu {
    // std::function<void(std::chrono::milliseconds, bool, raylib::Color)> blinker =
    //     make_timed_state_machine([this](auto start_time, auto end_time, raylib::Color color) {
    //         float c = (float)(end_time - std::chrono::steady_clock::now()).count() /
    //                   (end_time - start_time).count();
    //         c *= c;
    //         m_borders.Draw(raylib::Color(color.r * c, color.g * c, color.b * c, color.a * c));
    //     });
    //

    static constexpr int stat_screen_height = GuiBoard::height;
    static constexpr int stat_screen_width = GuiBoard::width;
    static constexpr int offset_top = GuiBoard::offset_top;

    enum class StatisticKind { LEVEL, STRENGTH, BONUS, COUNT };

    struct Statistic {
        std::string name;
        int value;
    };

    EnumArray<StatisticKind, Statistic> menu_elements = {
        {StatisticKind::LEVEL, {"Level", 0}},
        {StatisticKind::STRENGTH, {"Strength", 0}},
        {StatisticKind::BONUS, {"Bonus", 0}}};

    void draw() {
    }
};

}  // namespace meow

#endif  // GUI_PLAYER_STATISTICS_MENU_HPP_
