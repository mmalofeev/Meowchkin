#include "gui_board.hpp"
#include <chrono>
#include <iostream>
#include <memory>
#include "game_session.hpp"
#include "gui_card_span_dropdown_menu.hpp"
#include "message_types.hpp"
#include "paths_to_binaries.hpp"
#include "timed_state_machine.hpp"

namespace meow {

void GuiBoard::setup(
    raylib::Window *window,
    GuiCardSpan *hand,
    network::Client *client,
    model::GameSession *game_session
) {
    if (window == nullptr || hand == nullptr || client == nullptr) {
        throw std::invalid_argument("invalid pointer setted in gui board!");
    }
    m_player_hand = hand;
    m_window = window;
    m_client = client;
    m_game_session = game_session;

    for (const auto &info : client->get_players_info()) {
        if (info.id == client->get_id_of_client()) {
            m_kitten_cards[info.id] = std::make_unique<KittenCardsDDM>(&m_kitten_cards[info.id]);

        } else {
            m_kitten_cards[info.id] = std::make_unique<BrawlCardsDDM>(&m_kitten_cards[info.id]);
        }
    }

    const raylib::Vector2 offset = {(m_window->GetWidth() - width) / 2.0f, offset_top};
    m_rect = raylib::Rectangle(offset.x, offset.y, width, height);
    const float rect_side = 10.0f;
    m_drop_card_rect = raylib::Rectangle(
        offset.x + (m_rect.width - rect_side) / 2, offset.y + (m_rect.height - rect_side) / 2,
        rect_side, rect_side
    );

    raylib::Image image;
    try {
        image.Load(board_image_path);
        image.Resize(m_rect.GetSize().x, m_rect.GetSize().y);
    } catch (const raylib::RaylibException &) {
        image =
            raylib::Image::Color(m_rect.GetSize().x, m_rect.GetSize().y, raylib::Color::RayWhite());
    }
    m_texture.Load(image);

    for (auto &kc : m_kitten_cards) {
        kc.second.set_span_borders(m_rect, {m_rect.x, 0});
        kc.second.set_window(m_window);
    }
    m_opponent_cards.set_span_borders(m_rect, {m_rect.x, m_rect.height / 2 + 30});
    m_opponent_cards.set_window(m_window);
}

void GuiBoard::draw(
    std::size_t observed_player,
    float frame_time,
    std::optional<std::size_t> forced_target
) {
    static int color1 = 0xFFFFFFED;
    static int color2 = 0xFFFFFFFF;
    static auto bebra = make_timed_state_machine([this, frame_time](auto, auto) {
        color1 = (int)(color1 + (color2 - color1) * frame_time);
    });

    if (m_player_hand->selected()) {
        color2 = 0xFFFFFFFF;
    } else {
        color2 = 0xFFFFFFBB;
    }
    bool activate_stuff = true;
    bebra(std::chrono::milliseconds(1000), activate_stuff);

    m_texture.Draw(m_rect.GetPosition(), raylib::Color(color1));
    m_rect.DrawLines(raylib::Color::RayWhite(), 5);
    raylib::Vector2(m_rect.x, m_rect.y + m_rect.height / 2.0f)
        .DrawLine({m_rect.x + m_rect.width, m_rect.y + m_rect.height / 2.0f}, raylib::Color::Red());

    m_drop_card_rect.DrawLines(raylib::Color::Green());

    if (m_player_hand->selected().has_value()) {
        for (const GuiCardInfo &c : GuiCardSpan::possible_targets) {
            if (c.intersect.CheckCollision(m_player_hand->selected().value()->border)) {
                add_card(m_player_hand->pop_selected().card_id, c.card_id);
                return;
            }
        }
    }

    if (m_player_hand->selected().has_value() &&
        m_drop_card_rect.CheckCollision(m_player_hand->selected().value()->border)) {
        add_card(
            m_player_hand->pop_selected().card_id,
            forced_target ? m_game_session->get_player_id_by_user_id(*forced_target)
                          : m_game_session->get_player_id_by_user_id(m_client->get_id_of_client())
        );
    }

    m_kitten_cards.at(observed_player).draw_cards(frame_time);

    // std::cout << m_opponent_cards.m_cards.size() << std::endl;
    m_opponent_cards.draw_cards(frame_time);
}

void GuiBoard::add_card(std::size_t card_id, std::size_t target_id) {
    m_client->send_action(network::Action(
        network::Action::ActionType::PlayedCard, card_id, target_id, m_client->get_id_of_client()
    ));
}

}  // namespace meow
