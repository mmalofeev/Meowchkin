#ifndef GUI_CARD_SPAN_HPP_
#define GUI_CARD_SPAN_HPP_

#include <algorithm>
#include <gui_window_dependable.hpp>
#include <list>
#include <memory>
#include <optional>
#include "gui_card.hpp"
#include "noncopyable.hpp"
#include "raygui.h"
#include "raylib-cpp.hpp"

namespace meow {

class DropDownMenu;

class GuiCardSpan : Noncopyable, WindowDependable<GuiCardSpan> {
    friend class DropDownMenu;

private:
    struct RemovedGuiCard {
        GuiCard card;
        double fading_coeff;
    };

    int m_card_gap = 50;
    raylib::Window *m_window = nullptr;
    std::list<GuiCard> m_cards;
    std::list<RemovedGuiCard> m_removed_cards;
    std::list<GuiCard>::iterator m_selected = m_cards.end();
    raylib::Rectangle m_span_borders;
    raylib::Vector2 m_offset;

    std::unique_ptr<DropDownMenu> m_dropdown_menu;

public:
    explicit GuiCardSpan(std::unique_ptr<DropDownMenu> ddm) : m_dropdown_menu(std::move(ddm)) {
    }

    [[nodiscard]] std::size_t card_count() const noexcept {
        return m_cards.size();
    }

    void set_window(raylib::Window *window) noexcept {
        m_window = window;
    }

    void set_span_borders(
        const raylib::Rectangle &borders,
        const raylib::Vector2 &offset = {0, 30}
    ) noexcept {
        m_span_borders = borders;
        m_offset = offset;
    }

    [[nodiscard]] std::optional<std::list<GuiCard>::iterator> selected() const noexcept {
        if (m_selected == m_cards.end()) {
            return std::nullopt;
        }
        return m_selected;
    }

    [[nodiscard]] GuiCard pop_selected() {
        if (m_selected == m_cards.end()) {
            throw std::runtime_error("invalid pop selected from hand!");
        }
        GuiCard ret = std::move(*m_selected);  // card texture is move-only
        m_cards.erase(m_selected);
        m_selected = m_cards.end();
        m_card_gap += 10;
        recalculate_card_rects();
        return ret;
    }

    void recalculate_card_rects() noexcept;
    void add_card(std::string_view path_to_texture = "");
    void add_card(GuiCard &&card);
    void remove_card();
    void remove_card(std::list<GuiCard>::iterator card_iter);

    void remove_card(std::string_view card_filename) {
        auto it = std::find_if(m_cards.begin(), m_cards.end(), [card_filename](const GuiCard &c) {
            return c.filename == card_filename;
        });
        remove_card(it);
    }

    void remove_card(std::size_t card_id);
    void draw_cards(float frame_time, bool can_be_dragged);
};

}  // namespace meow

#endif  // GUI_CARD_SPAN_HPP_
