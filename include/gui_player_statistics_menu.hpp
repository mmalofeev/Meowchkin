#ifndef GUI_PLAYER_STATISTICS_MENU_HPP_
#define GUI_PLAYER_STATISTICS_MENU_HPP_

#include <chrono>
#include <functional>
#include <string>
#include "Rectangle.hpp"
#include "enum_array.hpp"
#include "gui_card.hpp"
#include "paths_to_binaries.hpp"
#include "raygui.h"
#include "raylib-cpp.hpp"
#include "timed_state_machine.hpp"

namespace meow {

class GuiPlayerStatisticsMenu {
    std::function<void(std::chrono::milliseconds, bool, raylib::Color)> blinker =
        make_timed_state_machine([this](auto start_time, auto end_time, raylib::Color color) {
            float c = (float)(end_time - std::chrono::steady_clock::now()).count() /
                      (end_time - start_time).count();
            c *= c;
            m_borders.Draw(raylib::Color(color.r * c, color.g * c, color.b * c, color.a * c));
        });

public:
    enum class StatisticKind { LEVEL, STRENGTH, BONUS, COUNT };

private:
    static constexpr unsigned stat_screen_height = GuiCard::height / (double)StatisticKind::COUNT;
    static constexpr unsigned stat_screen_width = GuiCard::width;

public:
    raylib::Rectangle m_borders = {0, 200, GuiCard::width, GuiCard::height};

    struct Statistic {
        std::string name;
        int value;
    };

    raylib::Color blink_color = raylib::Color::White();
    bool blink = false;

    EnumArray<StatisticKind, Statistic> menu_elements = {
        {StatisticKind::LEVEL, {"Level", 0}},
        {StatisticKind::STRENGTH, {"Strength", 0}},
        {StatisticKind::BONUS, {"Bonus", 0}}};

    void draw() {
        raylib::Vector2 cur_pos = m_borders.GetPosition();
        m_borders.Draw(raylib::Color::DarkGray());
        m_borders.DrawRoundedLines(0.1, 4, 10, raylib::Color::Blue());
        for (const auto &[name, val] : menu_elements.data()) {
            static const raylib::Font font = raylib::LoadFontEx(gui_font_path, 40, 0, 0);
            // raylib::Rectangle(cur_pos, {stat_screen_width, stat_screen_height})
            //     .DrawRounded(0.3, 4, raylib::Color(255, 255, 255, 200));
            raylib::DrawTextEx(
                font, name + ": " + std::to_string(val), cur_pos, 40.0f, 0.0f,
                raylib::Color::RayWhite()
            );
            cur_pos += raylib::Vector2(0, stat_screen_height);
        }
        blinker(std::chrono::milliseconds(1000), blink, blink_color);
        // blink is modified in outer scope, here it resets
        blink = false;
    }
};

}  // namespace meow

#endif  // GUI_PLAYER_STATISTICS_MENU_HPP_
