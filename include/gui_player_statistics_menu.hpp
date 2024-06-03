#ifndef GUI_PLAYER_STATISTICS_MENU_HPP_
#define GUI_PLAYER_STATISTICS_MENU_HPP_

#include <string>
#include <unordered_map>
#include <vector>
#include "enum_array.hpp"
#include "raygui.h"
#include "raylib-cpp.hpp"

namespace meow {

class GuiPlayerStatisticsMenu {
    static constexpr float width = 150.0f;
    static constexpr float height = 50.0f;

public:
    enum class StatisticKind { LEVEL, STRENGTH, BONUS, MONSTER_STRENGTH, COUNT };

    struct Statistic {
        std::string name;
        int value;
    };

    using element_t = EnumArray<StatisticKind, Statistic>;
    std::unordered_map<std::size_t, element_t> elements;

private:
    std::vector<std::pair<raylib::Rectangle, raylib::Rectangle>> m_rects;
    raylib::Rectangle m_whole_rect;

public:
    GuiPlayerStatisticsMenu()
        : m_rects(element_t::size()), m_whole_rect(0, 0, 2 * width, element_t::size() * height) {
        for (std::size_t i = 0; i < m_rects.size(); ++i) {
            m_rects[i].first = raylib::Rectangle{0, i * height, width, height};
            m_rects[i].second = raylib::Rectangle{width, i * height, width, height};
        }
    }

    void draw(std::size_t observed_player) {
        m_whole_rect.Draw(raylib::Color(0x00000088));
        auto &elements_ = elements.at(observed_player);
        for (std::size_t i = 0; i < element_t::size(); ++i) {
            m_rects[i].first.DrawLines(raylib::Color::Gray());
            raylib::DrawText(
                elements_[i].name, m_rects[i].first.x, m_rects[i].first.y, 20,
                raylib::Color::White()
            );

            m_rects[i].second.DrawLines(raylib::Color::Gray());
            raylib::DrawText(
                std::to_string(elements_[i].value), m_rects[i].second.x, m_rects[i].second.y, 20,
                raylib::Color::White()
            );
        }
    }
};

}  // namespace meow

#endif  // GUI_PLAYER_STATISTICS_MENU_HPP_
