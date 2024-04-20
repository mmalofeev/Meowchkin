#include "raylib_game_view.hpp"
#include <iostream>
#include "gui_dice_roller.hpp"
#include "paths_to_binaries.hpp"

meow::RaylibGameView::RaylibGameView() {
    GuiLoadStyle(gui_style_path);
    GuiSetFont(LoadFont(gui_font_path));
    GuiSetStyle(DEFAULT, TEXT_SIZE, 16);
    for (const auto &entry : std::filesystem::directory_iterator(cards_directory_path)) {
        if (entry.path().extension() == ".png") {
            m_card_image_paths.emplace_back(entry);
        }
    }
}

void meow::RaylibGameView::on_instances_attach() {
    setup_background();
    setup_pause_menu();
    setup_hand();
    m_board.setup(m_window, &m_player_hand, m_client);
    m_text_chat.set_window(m_window);
    m_usernames_box.set_window(m_window);

    for (const auto &info : m_client->get_players_info()) {
        m_usernames_box.add_username(info.name);
    }
}

void meow::RaylibGameView::setup_background() {
    raylib::Image background_image;
    try {
        background_image.Load(gameview_background_image_path);
        background_image.Resize(m_window->GetWidth(), m_window->GetHeight());
    } catch (const raylib::RaylibException &) {
        background_image =
            raylib::Image::Color(m_window->GetWidth(), m_window->GetHeight(), background_color);
    }
    m_background.Load(background_image);

    raylib::Image blur = raylib::Image::GradientRadial(
        m_window->GetWidth(), m_window->GetHeight(), 0, raylib::Color::Blank(), {0, 0, 0, 100}
    );
    m_blur.Load(blur);
}

void meow::RaylibGameView::setup_pause_menu() {
    for (std::size_t i = 0; i < m_pause_button_rects.size(); i++) {
        m_pause_button_rects[i].width = button_width;
        m_pause_button_rects[i].height = button_height;
        m_pause_button_rects[i].x = m_window->GetWidth() / 2 - button_width / 2;
        m_pause_button_rects[i].y = m_window->GetHeight() / 2 + i * button_height;
    }
}

void meow::RaylibGameView::setup_hand() {
    m_player_hand.set_window(m_window);
    m_player_hand.set_span_borders({m_window->GetPosition(), m_window->GetSize()});
    for (int i = 0; i < 5; i++) {
        const auto random_index = random_integer<std::size_t>(0, m_card_image_paths.size() - 1);
        m_player_hand.add_card(m_card_image_paths[random_index].c_str());
    }
}

void meow::RaylibGameView::draw_pause_menu() {
    for (std::size_t i = 0; i < m_pause_button_labels.size(); i++) {
        m_pause_button_pressed[i] = GuiButton(m_pause_button_rects[i], m_pause_button_labels[i]);
    }
    if (m_pause_button_pressed[PauseButton::CONTINUE] || IsKeyPressed(KEY_ESCAPE)) {
        m_should_draw_pause = false;
    }
    if (m_pause_button_pressed[PauseButton::BACK_TO_LOBBY]) {
        m_should_draw_pause = false;
        m_scene_manager->switch_scene(SceneType::MAIN_MENU);
    }
    if (m_pause_button_pressed[PauseButton::QUIT]) {
        m_should_draw_pause = false;
        m_running = false;
    }
}

void meow::RaylibGameView::draw() {
    if (m_window == nullptr) {
        throw std::runtime_error("invalid attached window!");
    } else if (m_client == nullptr) {
        throw std::runtime_error("invalid attached client!");
    }

    if (auto action = m_client->receive_action(); action) {
        // std::cout << "received " << action->card_filename << '\n';
        m_board.m_active_cards.add_card(action->card_filename);
    }
    if (auto chat_message = m_client->receive_chat_message(); chat_message) {
        std::cout << "received from " << chat_message->sender_player << ": "
                  << chat_message->message << '\n';
        m_text_chat.receive(*chat_message);
    }

    if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_ENTER)) {
        m_show_chat = !m_show_chat;
    }

    m_background.Draw();
    m_board.draw(m_window->GetFrameTime(), !m_should_draw_pause);
    m_player_hand.draw_cards(m_window->GetFrameTime(), !m_should_draw_pause);

    if (!m_should_draw_pause && GuiButton({0, 0, 40, 40}, "-") && m_player_hand.card_count() > 0) {
        m_player_hand.remove_card();
    }
    if (!m_should_draw_pause && GuiButton({40, 0, 40, 40}, "+") &&
        m_player_hand.card_count() < 10) {
        const auto random_index = random_integer<std::size_t>(0, m_card_image_paths.size() - 1);
        m_player_hand.add_card(m_card_image_paths[random_index].c_str());
    }
    GuiStatusBar({0, (float)m_window->GetHeight() - 30, 100, 30}, "status bar....");

    m_usernames_box.draw();
    if (m_show_chat) {
        if (m_text_chat.get_border().CheckCollision(raylib::Mouse::GetPosition()) &&
            raylib::Mouse::IsButtonDown(MOUSE_BUTTON_LEFT)) {
            m_text_chat.position += raylib::Mouse::GetDelta();
        }
        m_text_chat.draw(*m_client);
    }
    if (!m_should_draw_pause) {
        m_should_draw_pause = IsKeyPressed(KEY_ESCAPE);
    } else if (m_should_draw_pause) {
        m_blur.Draw();
        draw_pause_menu();
    }

    if (IsKeyPressed(KEY_SPACE)) {
        const auto random_index = random_integer<std::size_t>(0, m_card_image_paths.size() - 1);
        for (auto &info : m_client->get_players_info()) {
            std::cout << "send to " << info.name << '\n';
            m_client->send_action(network::Action(
                m_card_image_paths[random_index].c_str(), info.id, m_client->get_id_of_client()
            ));
        }
    }
}

/* Callbacks */
void meow::RaylibGameView::on_card_draw(std::string_view card_filename) {
}

void meow::RaylibGameView::on_turn_begin() {
}

void meow::RaylibGameView::on_turn_end() {
}

void meow::RaylibGameView::on_levelup() {
}  // maybe?..

void meow::RaylibGameView::on_card_receive() {
}

void meow::RaylibGameView::on_item_equip() {
}

void meow::RaylibGameView::on_item_loss() {
}

void meow::RaylibGameView::on_monster_elimination() {
}

void meow::RaylibGameView::on_being_cursed() {
}

extern "C" BOOST_SYMBOL_EXPORT meow::RaylibGameView game_view;
meow::RaylibGameView game_view;
