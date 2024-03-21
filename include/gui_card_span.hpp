#ifndef GUI_CARD_SPAN_HPP_
#define GUI_CARD_SPAN_HPP_

#include <gui_window_dependable.hpp>
#include <list>
#include <optional>
#include "enum_array.hpp"
#include "gui_card.hpp"
#include "raygui.h"
#include "raylib-cpp.hpp"

namespace meow {

class GuiCardSpan : WindowDependable<GuiCardSpan> {
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

    class DropDownMenu {
    private:
        static constexpr int button_width = 150;
        static constexpr int button_height = 33;
        GuiCardSpan &m_parental_span;
        std::list<GuiCard> &m_cards = m_parental_span.m_cards;
        std::list<GuiCard>::iterator m_card_iter = m_cards.end();
        enum class Button { INSPECT, REMOVE, COUNT };
        const EnumArray<Button, Rectangle> m_origin_button_rects{
            {Button::INSPECT, {0, 0, button_width, button_height}},
            {Button::REMOVE, {0, button_height, button_width, button_height}},
        };
        EnumArray<Button, Rectangle> m_button_rects = m_origin_button_rects;
        const EnumArray<Button, const char *> m_button_labels{
            {Button::INSPECT, "Inspect"},
            {Button::REMOVE, "Remove"}};

    public:
        explicit DropDownMenu(GuiCardSpan &parental_span) : m_parental_span(parental_span) {
        }

        [[nodiscard]] bool mouse_in_menu() const noexcept {
            const auto &arr = m_button_rects.data();
            raylib::Rectangle rec = {
                arr[0].x, arr[0].y, arr.back().x + button_width, arr.back().y + button_height};
            return rec.CheckCollision(raylib::Mouse::GetPosition());
        }

        void draw() {
            if (m_card_iter == m_cards.end()) {
                return;
            }

            EnumArray<Button, bool> pressed;
            GuiSetAlpha(0.85f);
            for (std::size_t i = 0; i < m_button_rects.size(); i++) {
                pressed[i] = GuiButton(m_button_rects[i], m_button_labels[i]);
            }
            GuiSetAlpha(1.0f);
            if (pressed[Button::REMOVE]) {
                m_parental_span.remove_card(m_card_iter);
                m_card_iter = m_cards.end();
            }
            // if (pressed[Button::INSPECT]) {
            //     std::swap(m_card_iter->full_sized_texture, m_card_iter->texture);
            //     std::swap(m_card_iter->full_sized_border, m_card_iter->border);
            // }
        }

        void detach_card() noexcept {
            m_card_iter = m_cards.end();
        }

        void attach_card(std::list<GuiCard>::iterator card_iter, const raylib::Vector2 &mouse) {
            m_card_iter = card_iter;
            for (std::size_t i = 0; i < m_origin_button_rects.size(); i++) {
                m_button_rects[i].x = m_origin_button_rects[i].x + mouse.x;
                m_button_rects[i].y = m_origin_button_rects[i].y + mouse.y;
            }
        }
    };

    DropDownMenu m_dropdown_menu = DropDownMenu(*this);

public:
    explicit GuiCardSpan() noexcept = default;

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
    void draw_cards(float frame_time, bool can_be_dragged);
};

}  // namespace meow

#endif  // GUI_CARD_SPAN_HPP_
