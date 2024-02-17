#include "gui_card_span.hpp"

namespace meow {

void GuiCardSpan::recalculate_card_rects() noexcept {
    // some magic for proper aligment
    // scaling ratio
    const double ratio = std::sqrt(m_span_borders.height / m_window->GetHeight());
    m_card_gap *= ratio * ratio;
    // delta between original card width and scaled card
    const double dx = (GuiCard::width - ratio * GuiCard::width) / 2;
    const int offsetx_from_borders =
        (m_span_borders.width - m_cards.size() * (GuiCard::width + m_card_gap) + m_card_gap) / 2;
    int i = 0;
    for (GuiCard &card : m_cards) {
        card.target_position.x =
            dx + m_offset.x + offsetx_from_borders + i * (GuiCard::width + m_card_gap);
        card.border.y = card.target_position.y =
            m_span_borders.height - (GuiCard::height * ratio + m_offset.y);
        card.border.width = GuiCard::width * ratio;
        card.border.height = GuiCard::height * ratio;
        i++;
    }
}

void GuiCardSpan::add_card(std::string_view path_to_texture) {
    raylib::Image img;
    try {
        img.Load(path_to_texture.data());
        img.Mipmaps();
        img.Resize(GuiCard::width, GuiCard::height);
    } catch (const raylib::RaylibException &) {
        img = raylib::Image::Color(GuiCard::width, GuiCard::height, raylib::Color::Green());
    }
    raylib::Texture tex = raylib::Texture(img);
    tex.GenMipmaps();
    add_card({raylib::Rectangle(m_window->GetWidth(), 0, 0, 0), raylib::Vector2(0), std::move(tex)}
    );
}

void GuiCardSpan::add_card(GuiCard &&card) {
    m_cards.push_back(std::move(card));
    m_card_gap -= 10;
    recalculate_card_rects();
}

void GuiCardSpan::remove_card() {
    m_removed_cards.push_back({std::move(m_cards.back()), 0});
    m_removed_cards.back().first.target_position =
        raylib::Vector2(-GuiCard::width, -GuiCard::height);
    m_removed_cards.back().second = (m_removed_cards.back().first.border.GetPosition() -
                                     m_removed_cards.back().first.target_position)
                                        .LengthSqr();
    m_cards.pop_back();
    m_card_gap += 10;
    recalculate_card_rects();
}

void GuiCardSpan::draw_cards(float frame_time, bool is_pause) {
    m_span_borders.DrawLines(raylib::Color::Red(), 3);
    if (is_pause ||
        m_selected != m_cards.end() && raylib::Mouse::IsButtonReleased(MOUSE_BUTTON_LEFT)) {
        m_selected = m_cards.end();
    }
    for (auto card_it = m_cards.begin(); card_it != m_cards.end(); card_it++) {
        if (card_it->border.CheckCollision(raylib::Mouse::GetPosition())) {
            if (!is_pause && m_selected == m_cards.end() &&
                raylib::Mouse::IsButtonDown(MOUSE_BUTTON_LEFT)) {
                m_selected = card_it;
            }
        }
    }

    // ratio is for texture; maybe i should store ratio as class member
    const double ratio = std::sqrt(m_span_borders.height / m_window->GetHeight());
    for (auto card_it = m_cards.begin(); card_it != m_cards.end(); card_it++) {
        if (card_it == m_selected) {
            continue;
        }
        if (m_selected == m_cards.end()) {
            card_it->border.DrawRounded(0.1f, 4, raylib::Color(0x004400FF));
        } else {
            card_it->border.DrawRounded(0.1f, 4, raylib::Color(0x00440088));
        }
        card_it->texture.Draw(
            card_it->border.GetPosition(), 0, ratio, raylib::Color(255, 255, 255, 200)
        );
        card_it->border.DrawRoundedLines(0.1f, 4, 3, raylib::Color::White());
        card_it->border.SetPosition(
            Vector2Lerp(card_it->border.GetPosition(), card_it->target_position, frame_time * 4)
        );
    }
    if (m_selected != m_cards.end()) {
        m_selected->border.SetPosition(
            m_selected->border.GetPosition() + raylib::Mouse::GetDelta()
        );
        m_selected->border.SetSize(m_selected->border.GetSize() * 5.0 / 4);
        m_selected->border.y -= m_selected->border.height * 1.0 / 5;
        m_selected->texture.Draw(m_selected->border.GetPosition(), 0, 5.0 / 4 * ratio);
        m_selected->border.DrawRoundedLines(0.1f, 4, 3, raylib::Color::Red());
        m_selected->border.y += m_selected->border.height * 1.0 / 5;
        m_selected->border.SetSize(m_selected->border.GetSize() * 4.0 / 5);
    }

    for (auto card_it = m_removed_cards.begin(); card_it != m_removed_cards.end(); card_it++) {
        auto &card = *card_it;
        raylib::Color c(
            255, 255, 255,
            (unsigned char)((card.first.border.GetPosition() - card.first.target_position)
                                .LengthSqr() /
                            card.second * 255)
        );
        card.first.border.SetPosition(
            Vector2Lerp(card.first.border.GetPosition(), card.first.target_position, frame_time * 4)
        );
        card.first.texture.Draw(card.first.border.GetPosition(), c);
        if (c.a == 0) {
            card_it = m_removed_cards.erase(card_it);
        }
    }
}

}  // namespace meow
