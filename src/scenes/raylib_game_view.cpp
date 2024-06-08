#include "raylib_game_view.hpp"
#include <boost/dll/alias.hpp>
#include <chrono>
#include <cstddef>
#include <ios>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <string_view>
#include <variant>
#include "Rectangle.hpp"
#include "gui_card_loader.hpp"
#include "gui_card_span_dropdown_menu.hpp"
#include "gui_dice_roller.hpp"
#include "message_types.hpp"
#include "paths_to_binaries.hpp"
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

namespace {

constexpr float opts_button_width = 210;
constexpr float opts_button_height = 70;

template <typename... Ts>
struct Overload : Ts... {
    using Ts::operator()...;
};

template <typename EnumT>
    requires meow::CountableEnum<EnumT>
EnumT draw_opts_menu(const meow::EnumArray<EnumT, std::string> &opts, raylib::Vector2 pos) {
    EnumT ret = EnumT::COUNT;
    for (std::size_t i = 0; i < opts.size(); ++i) {
        if (GuiButton(
                Rectangle{
                    pos.x, pos.y + i * opts_button_height, opts_button_width, opts_button_height},
                opts[i].c_str()
            )) {
            ret = static_cast<EnumT>(i);
        }
    }
    return ret;
}

}  // namespace

meow::RaylibGameView::RaylibGameView()
    : m_active_display(&m_gameplay_objects), m_on_levelup_texture("bin/imgs/levelup.png") {
    GuiLoadStyle(gui_style_path);
    GuiSetFont(LoadFont(gui_font_path));
    for (const auto &entry : std::filesystem::directory_iterator(cards_directory_path)) {
        if (entry.path().extension() == ".png") {
            load_card_img(entry.path().c_str());
            m_card_image_paths.emplace_back(entry);
        }
    }
    m_gameplay_objects.player_hand =
        std::make_unique<PlayerHandDDM>(&m_gameplay_objects.player_hand);
    m_background_music.Play();
    m_background_music.SetLooping(true);

    m_endgame_screen.texture[EndGameScreen::GameResult::WIN] = raylib::Texture(
        raylib::Image(path_to_win_screen).Resize(m_window->GetWidth(), m_window->GetHeight())
    );
    m_endgame_screen.texture[EndGameScreen::GameResult::LOOSE] = raylib::Texture(
        raylib::Image(path_to_loose_screen).Resize(m_window->GetWidth(), m_window->GetHeight())
    );
}

void meow::RaylibGameView::on_instances_attach() {
    setup_background();

    setup_pause_menu();

    setup_hand();

    setup_active_display_selector();

    GuiCardSpan::card_manager = card_manager;
    GuiCardSpan::m_client = m_client;

    m_gameplay_objects.board.setup(
        m_window, &m_gameplay_objects.player_hand, m_client, game_session
    );

    // const auto clients_info = m_client->get_players_info();

    // m_gameplay_objects.opponent_checkbox.resize(clients_info.size());

    m_gameplay_objects.text_chat.set_window(m_window);

    m_gameplay_objects.usernames_box.set_window(m_window);

    m_gameplay_objects.usernames_box.active_user = m_client->get_id_of_client();
    m_player_id = game_session->get_player_id_by_user_id(m_client->get_id_of_client());

    const float x0 = m_window->GetWidth() - (m_window->GetWidth() - GuiBoard::width) / 2.0f + 5.0f;
    const float y0 = GuiBoard::offset_top + GuiBoard::height / 2.0f -
                     20.0f * m_client->get_players_info().size();
    std::size_t i = 0;
    for (const auto &info : m_client->get_players_info()) {
        m_gameplay_objects.usernames_box.add_username({info.id, info.name});
        m_gameplay_objects.stats.elements[info.id] = {
            {GuiPlayerStatisticsMenu::StatisticKind::LEVEL, {"Level", 1}},
            {GuiPlayerStatisticsMenu::StatisticKind::STRENGTH, {"Strength", 1}},
            {GuiPlayerStatisticsMenu::StatisticKind::BONUS, {"Bonus", 0}},
            {GuiPlayerStatisticsMenu::StatisticKind::MONSTER_STRENGTH, {"Monster\nstrength", 0}},
            {GuiPlayerStatisticsMenu::StatisticKind::LAST_DICE_ROLL, {"Your last dice\nroll", 0}},
        };
        m_gameplay_objects.opponent_checkbox[info.id] = raylib::Rectangle{x0, y0 + i * 40, 40, 40};
        ++i;
    }

    m_client->send_action(network::Action(
        network::Action::ActionType::RollDice, -1, m_client->get_id_of_client(),
        m_client->get_id_of_client()
    ));
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

    raylib::Image blur2 = raylib::Image::GradientRadial(
        m_window->GetWidth(), m_window->GetHeight(), 0, raylib::Color::Blank(), {0, 158, 47, 100}
    );
    m_on_levelup_blur_texture.Load(blur2);
}

void meow::RaylibGameView::setup_pause_menu() {
    raylib::Rectangle *rec = &m_pause_menu_objects.button_rects[0];
    for (std::size_t i = 0; i < m_pause_menu_objects.button_rects.size(); ++i) {
        m_pause_menu_objects.button_rects[i].width = PauseMenuObjs::button_width;
        m_pause_menu_objects.button_rects[i].height = PauseMenuObjs::button_height;
        m_pause_menu_objects.button_rects[i].x =
            m_window->GetWidth() / 2.0f - PauseMenuObjs::button_width / 2.0f;
        m_pause_menu_objects.button_rects[i].y =
            m_window->GetHeight() / 2.0f + i * PauseMenuObjs::button_height;
        rec = &m_pause_menu_objects.button_rects[i];
    }
    m_pause_menu_objects.music_volume_rec = *rec;
    m_pause_menu_objects.music_volume_rec.y += PauseMenuObjs::button_height;
}

void meow::RaylibGameView::setup_active_display_selector() {
    auto &br = m_active_display_selector.button_rects;
    auto &texs = m_active_display_selector.button_texs;

    float height = 0;
    const int w = ActiveDisplaySelector::button_width, h = ActiveDisplaySelector::button_height;
    for (std::size_t i = 0; i < br.size(); ++i) {
        br[i].width = w;
        br[i].height = h;
        br[i].x = (float)(m_window->GetWidth() - GuiBoard::width) / 2 - h;
        height = br[i].y = i * h;

        try {
            texs[i] = raylib::Image(m_active_display_selector.button_texture_paths[i]).Resize(w, h);
        } catch (const raylib::RaylibException &) {
            texs[i] = raylib::Image().Color(w, h, raylib::Color(random_integer<unsigned>()));
        }
    }

    // align with board center
    height += h;
    for (std::size_t i = 0; i < br.size(); ++i) {
        br[i].y = br[i].y - height / 2 + GuiBoard::offset_top + GuiBoard::height / 2.0f;
    }
}

void meow::RaylibGameView::setup_hand() {
    m_gameplay_objects.player_hand.set_window(m_window);
    m_gameplay_objects.player_hand.set_span_borders({m_window->GetPosition(), m_window->GetSize()});
    // for (int i = 0; i < 5; i++) {
    //     const auto random_index = random_integer<std::size_t>(0, m_card_image_paths.size() - 1);
    //     m_gameplay_objects.player_hand.add_card(m_card_image_paths[random_index].c_str());
    // }
}

void meow::RaylibGameView::draw_pause_menu() {
    using PauseButton = PauseMenuObjs::Button;
    for (std::size_t i = 0; i < m_pause_menu_objects.button_labels.size(); ++i) {
        m_pause_menu_objects.button_pressed[i] =
            GuiButton(m_pause_menu_objects.button_rects[i], m_pause_menu_objects.button_labels[i]);
    }

    if (m_pause_menu_objects.button_pressed[PauseButton::CONTINUE]) {
        m_active_display = &m_gameplay_objects;
    }
    if (m_pause_menu_objects.button_pressed[PauseButton::BACK_TO_LOBBY]) {
        m_scene_manager->switch_scene(SceneType::MAIN_MENU);
    }
    if (m_pause_menu_objects.button_pressed[PauseButton::QUIT]) {
        m_running = false;
    }

    GuiSliderBar(
        m_pause_menu_objects.music_volume_rec, "", "", &m_pause_menu_objects.music_volume, 0.0f,
        1.0f
    );
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, raylib::Color::Gray().ToInt());
    GuiLabel(m_pause_menu_objects.music_volume_rec, "Music volume");
}

void meow::RaylibGameView::draw_active_display_selector() {
    auto &br = m_active_display_selector.button_rects;
    auto &texs = m_active_display_selector.button_texs;
    auto &pressed = m_active_display_selector.button_pressed;

    float was = guiAlpha;
    GuiSetAlpha(0.3f);
    for (std::size_t i = 0; i < br.size(); ++i) {
        br[i].DrawRoundedLines(0.1, 4, 1, raylib::Color::RayWhite());
        br[i].DrawRounded(0.1, 4, raylib::Color(0xFFFFFF88));
        texs[i].Draw(br[i].GetPosition());
        pressed[i] = GuiButton(br[i], "");
    }
    GuiSetAlpha(was);

    using Button = ActiveDisplaySelector::Button;
    if (pressed[Button::BRAWL]) {
        m_show_players = !m_show_players;
    } else if (pressed[Button::PAUSE]) {
        m_active_display = &m_pause_menu_objects;
    } else if (pressed[Button::CHAT]) {
        m_show_chat = !m_show_chat;
    } else if (pressed[Button::STATS]) {
        m_show_stats = !m_show_stats;
    }
}

void meow::RaylibGameView::draw() {
    using namespace meow::network;
    using ActionType = Action::ActionType;

    m_background_music.SetVolume(m_pause_menu_objects.music_volume);
    m_background_music.Update();
    static const Overload active_display_visitor = {
        [this](GameplayObjs *objs) {
            const bool my_turn = m_active_turn[m_client->get_id_of_client()];
            std::size_t id = objs->usernames_box.active_user;

            if (auto feedback = m_client->receive_action_result();
                feedback && !feedback->validness) {
                if (feedback->failed_action_type == ActionType::PlayedCard) {
                    m_gameplay_objects.player_hand.add_card(feedback->card_id);
                    m_incorrect_beep_sound.Play();
                }
            }

            objs->board.draw(
                id, m_window->GetFrameTime(),
                m_gameplay_objects.selected_opponent != std::numeric_limits<std::size_t>::max()
                    ? std::optional<std::size_t>(m_gameplay_objects.selected_opponent)
                    : std::nullopt
            );

            objs->player_hand.draw_cards(m_window->GetFrameTime(), true);

            if (m_show_players) {
                objs->usernames_box.draw(m_client->get_id_of_client());
            }

            if (m_show_chat) {
                objs->text_chat.draw(*m_client);
            }

            if (m_show_stats) {
                objs->stats.draw(id);
            }

            draw_active_display_selector();

            GuiCardSpan::draw_inspected_card(m_window->GetWidth(), m_window->GetHeight());

            if (GuiButton({0, m_window->GetHeight() - 40.0f, 40, 40}, "-") &&
                objs->player_hand.card_count() > 0) {
                objs->player_hand.remove_card();
            }

            if (GuiButton({40, m_window->GetHeight() - 40.0f, 40, 40}, "+") &&
                objs->player_hand.card_count() < 10) {
                m_client->send_action(Action(
                    ActionType::DrawedCard, random_integer(0, 1), -1, m_client->get_id_of_client()
                ));
            }
            if (objs->player_hand.somethind_inspected()) {
                m_card_draw_sound.Play();
                m_blur.Draw();
            }

            if (my_turn) {
                enum class opts { ROLL_DICE, PASS, END_TURN, COUNT };
                EnumArray<opts, std::string> turn_opts{
                    {opts::ROLL_DICE, "Roll dice"},
                    {opts::PASS, "Pass"},
                    {opts::END_TURN, "End turn"}};
                EnumArray<opts, ActionType> turn_acts{
                    {opts::ROLL_DICE, ActionType::RollDice},
                    {opts::PASS, ActionType::Pass},
                    {opts::END_TURN, ActionType::EndTurn}};
                float was = guiAlpha;
                GuiSetAlpha(0.85);
                if (auto x = draw_opts_menu(
                        turn_opts, {m_window->GetWidth() - opts_button_width,
                                    m_window->GetHeight() - opts_button_height * turn_opts.size()}
                    );
                    x != opts::COUNT) {
                    m_client->send_action(Action(
                        turn_acts[x], -1, m_client->get_id_of_client(), m_client->get_id_of_client()
                    ));
                }
                GuiSetAlpha(was);
            } else {
                enum class opts { ROLL_DICE, PASS, COUNT };
                EnumArray<opts, std::string> turn_opts{
                    {opts::ROLL_DICE, "Roll dice"}, {opts::PASS, "Pass"}};
                EnumArray<opts, ActionType> turn_acts{
                    {opts::ROLL_DICE, ActionType::RollDice}, {opts::PASS, ActionType::Pass}};
                float was = guiAlpha;
                GuiSetAlpha(0.85);
                if (auto x = draw_opts_menu(
                        turn_opts, {m_window->GetWidth() - opts_button_width,
                                    m_window->GetHeight() - opts_button_height * turn_opts.size()}
                    );
                    x != opts::COUNT) {
                    m_client->send_action(Action(
                        turn_acts[x], -1, m_client->get_id_of_client(), m_client->get_id_of_client()
                    ));
                }
                GuiSetAlpha(was);
            }

            for (const auto &info : m_client->get_players_info()) {
                if (GuiCheckBox(
                        m_gameplay_objects.opponent_checkbox[info.id], info.name.c_str(),
                        &m_gameplay_objects.opponent_checkbox_flags[info.id]
                    )) {
                    if (m_gameplay_objects.selected_opponent !=
                        std::numeric_limits<std::size_t>::max()) {
                        m_gameplay_objects
                            .opponent_checkbox_flags[m_gameplay_objects.selected_opponent] = false;
                        if (m_gameplay_objects.selected_opponent != info.id) {
                            m_gameplay_objects.opponent_checkbox_flags
                                [m_gameplay_objects.selected_opponent = info.id] = true;
                        } else {
                            m_gameplay_objects.selected_opponent =
                                std::numeric_limits<std::size_t>::max();
                        }
                    } else {
                        m_gameplay_objects.selected_opponent = info.id;
                        m_gameplay_objects.opponent_checkbox_flags[info.id] = true;
                    }
                }
            }
        },

        [this](PauseMenuObjs *objs) {
            m_blur.Draw();
            draw_pause_menu();
        },

        [this](EndGameScreen *screen) {
            m_endgame_screen.texture[m_endgame_screen.result].Draw();
            m_endgame_screen.sound[m_endgame_screen.result].Play();
        },

        [](auto...) { throw std::runtime_error("unhandled type(s) in active display!"); }};

    if (auto chat_message = m_client->receive_chat_message(); chat_message) {
        std::cout << "received from " << chat_message->sender_player << ": "
                  << chat_message->message << '\n';
        m_gameplay_objects.text_chat.receive(*chat_message);
    }

    m_background.Draw();

    std::visit(active_display_visitor, m_active_display);

    if (IsKeyPressed(KEY_SPACE)) {
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        if (!std::holds_alternative<PauseMenuObjs *>(m_active_display)) {
            m_active_display = &m_pause_menu_objects;
        } else {
            m_active_display = &m_gameplay_objects;
        }
    }
    m_levelup_blink_call(std::chrono::milliseconds(1000), m_levelup_blink, m_levelup_tint_color);
}

void meow::RaylibGameView::on_card_add_on_board(
    std::size_t card_id,
    bool protogonist_sided,
    std::size_t user_id
) {
    if (protogonist_sided) {
        dbg;
        m_gameplay_objects.board.m_kitten_cards.at(user_id).add_card(card_id);
    } else {
        m_gameplay_objects.board.m_opponent_cards.add_card(card_id);
    }
}

void meow::RaylibGameView::on_card_remove_from_board(std::size_t card_id) {
    dbg;
    m_gameplay_objects.board.remove_card(card_id);
}

void meow::RaylibGameView::on_turn_begin(std::size_t user_id) {
    m_active_turn[user_id] = true;
}

void meow::RaylibGameView::on_turn_end(std::size_t user_id) {
    m_active_turn[user_id] = false;
}

void meow::RaylibGameView::on_level_change(std::size_t user_id, int delta) {
    m_gameplay_objects.stats.elements[user_id][GuiPlayerStatisticsMenu::StatisticKind::LEVEL]
        .value += delta;
    m_gameplay_objects.stats.elements[user_id][GuiPlayerStatisticsMenu::StatisticKind::STRENGTH]
        .value += delta;
    if (user_id == m_client->get_id_of_client() && delta > 0) {
        m_levelup_sound.Play();
        m_levelup_blink = true;
        m_levelup_tint_color = raylib::Color::White();
    }
}

void meow::RaylibGameView::on_bonus_change(std::size_t user_id, int delta) {
    if (delta > 0 && user_id == m_client->get_id_of_client()) {
        m_item_equip_sound.Play();
    }
    m_gameplay_objects.stats.elements[user_id][GuiPlayerStatisticsMenu::StatisticKind::BONUS]
        .value += delta;
    m_gameplay_objects.stats.elements[user_id][GuiPlayerStatisticsMenu::StatisticKind::STRENGTH]
        .value += delta;
}

void meow::RaylibGameView::on_monster_bonus_change(std::size_t monster_id, int delta) {
    for (auto &element : m_gameplay_objects.stats.elements) {
        element.second[GuiPlayerStatisticsMenu::StatisticKind::MONSTER_STRENGTH].value += delta;
    }
}

void meow::RaylibGameView::on_card_receive(std::size_t user_id, size_t card_id) {
    if (user_id == m_client->get_id_of_client()) {
        m_gameplay_objects.player_hand.add_card(card_id);
        m_card_draw_sound.Play();
    }
}

void meow::RaylibGameView::on_card_loss(std::size_t user_id, std::size_t card_id) {
    if (user_id == m_client->get_id_of_client()) {
        m_gameplay_objects.player_hand.remove_card(card_id);
        m_card_draw_sound.Play();
    }
}

void meow::RaylibGameView::on_monster_elimination(std::size_t user_id) {
    if (user_id == m_client->get_id_of_client()) {
    }
}

void meow::RaylibGameView::on_dice_roll(std::size_t user_id, unsigned res) {
    m_gameplay_objects.stats
        .elements[user_id][GuiPlayerStatisticsMenu::StatisticKind::LAST_DICE_ROLL]
        .value = (int)res;
}

void meow::RaylibGameView::on_game_end(std::size_t winner_id) {
    if (m_client->get_id_of_client() == winner_id) {
        m_endgame_screen.result = EndGameScreen::GameResult::WIN;
    } else {
        m_endgame_screen.result = EndGameScreen::GameResult::LOOSE;
    }
    m_active_display = &m_endgame_screen;
}

BOOST_DLL_ALIAS(meow::RaylibGameView::make_raylib_gameview, make_gameview)
