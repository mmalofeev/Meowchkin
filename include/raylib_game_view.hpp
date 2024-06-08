#ifndef RAYLIB_GAME_VIEW_HPP_
#define RAYLIB_GAME_VIEW_HPP_

#include <chrono>
#include <filesystem>
#include <limits>
#include <memory>
#include <unordered_map>
#include <variant>
#include "enum_array.hpp"
#include "game_view.hpp"
#include "gui_board.hpp"
#include "gui_card_span.hpp"
#include "gui_player_statistics_menu.hpp"
#include "gui_text_chat.hpp"
#include "gui_usernames_box.hpp"
#include "paths_to_binaries.hpp"
#include "timed_state_machine.hpp"

namespace meow {

class RaylibGameView : public GameView {
private:
    inline static const raylib::Color background_color = raylib::Color(77, 120, 204, 255);

    struct GameplayObjs {
        GuiBoard board;
        GuiCardSpan player_hand;
        GuiUsernamesBox usernames_box;
        GuiTextChat text_chat;
        GuiPlayerStatisticsMenu stats;

        std::unordered_map<std::size_t, raylib::Rectangle> opponent_checkbox;
        std::unordered_map<std::size_t, bool> opponent_checkbox_flags;
        std::size_t selected_opponent = std::numeric_limits<std::size_t>::max();
    } m_gameplay_objects;

    struct PauseMenuObjs {
        static constexpr int button_width = 300;
        static constexpr int button_height = 40;

        enum class Button { CONTINUE, BACK_TO_LOBBY, QUIT, COUNT };
        EnumArray<Button, const char *> button_labels = {
            {Button::CONTINUE, "Continue"},
            {Button::BACK_TO_LOBBY, "Back to lobby"},
            {Button::QUIT, "Quit"},
        };
        EnumArray<Button, raylib::Rectangle> button_rects;
        EnumArray<Button, bool> button_pressed;

        float music_volume = 1.0f;
        raylib::Rectangle music_volume_rec;
    } m_pause_menu_objects;

    struct EndGameScreen {
        static constexpr int button_width = 300;
        static constexpr int button_height = 40;
        enum class GameResult { WIN, LOOSE, COUNT };

        GameResult result;
        EnumArray<GameResult, raylib::Texture> texture;
        EnumArray<GameResult, raylib::Sound> sound;
    } m_endgame_screen;

    std::variant<GameplayObjs *, PauseMenuObjs *, EndGameScreen *> m_active_display;

    struct ActiveDisplaySelector {
        static constexpr int button_width = 80;
        static constexpr int button_height = 80;

        enum class Button { BRAWL, STATS, CHAT, PAUSE, COUNT };
        EnumArray<Button, const char *> button_texture_paths = {
            {Button::BRAWL, path_to_brawl_icon},
            {Button::STATS, path_to_stats_icon},
            {Button::CHAT, path_to_chat_icon},
            {Button::PAUSE, path_to_pause_icon},
        };
        EnumArray<Button, raylib::Texture> button_texs;
        EnumArray<Button, raylib::Rectangle> button_rects;
        EnumArray<Button, bool> button_pressed;
    } m_active_display_selector;

    /* background */
    raylib::Texture m_background;
    raylib::Texture m_blur;

    /* misc */
    std::vector<std::filesystem::path> m_card_image_paths;
    bool m_show_chat = false;
    bool m_show_stats = true;
    bool m_show_players = true;
    raylib::Texture m_on_levelup_texture;
    raylib::Texture m_on_leveldown_texture;
    raylib::Texture m_on_levelup_blur_texture;
    raylib::Texture m_on_leveldown_blur_texture;
    raylib::Color m_levelup_tint_color = raylib::Color::White();
    state_machine_function_args_t<raylib::Color &> m_level_change_blink_call =
        make_timed_state_machine([this](auto st, auto end, raylib::Color &tint) {
            const int coeff =
                255.0f * (std::chrono::steady_clock::now() - st).count() / (end - st).count();
            if (coeff < 127) {
                tint.a = 127 + coeff;
            } else {
                tint.a = 255 - coeff % 127 * 2;
            }
            if (m_level_increased) {
                m_on_levelup_blur_texture.Draw(0, 0, tint);
                m_on_levelup_texture.Draw(
                    (m_window->GetWidth() - m_on_levelup_texture.width) / 2,
                    (m_window->GetHeight() - m_on_levelup_texture.height) / 2
                );
            } else {
                m_on_leveldown_blur_texture.Draw(0, 0, tint);
                m_on_leveldown_texture.Draw(
                    (m_window->GetWidth() - m_on_leveldown_texture.width) / 2,
                    (m_window->GetHeight() - m_on_leveldown_texture.height) / 2
                );
            }
        });
    bool m_levelup_blink = false;
    bool m_level_increased = false;
    raylib::Music m_background_music = raylib::Music(path_to_game_background_music);
    raylib::Sound m_levelup_sound = raylib::Sound(path_to_levelup_sound);
    raylib::Sound m_item_equip_sound = raylib::Sound(path_to_item_equip_sound);
    raylib::Sound m_incorrect_beep_sound = raylib::Sound(path_to_incorrect_beep_sound);
    raylib::Sound m_card_draw_sound = raylib::Sound(path_to_card_draw_sound);
    std::unordered_map<std::size_t, bool> m_active_turn;
    std::size_t m_player_id = -1;

protected:
    void on_instances_attach() override;

public:
    RaylibGameView();
    void draw() override;

    /* Callbacks */
    void on_card_add_on_board(
        std::size_t card_id,
        bool protogonist_sided,
        std::size_t user_id = std::numeric_limits<std::size_t>::max()
    ) override;
    void on_card_remove_from_board(std::size_t card_id) override;
    void on_turn_begin(std::size_t user_id) override;
    void on_turn_end(std::size_t user_id) override;
    void on_bonus_change(std::size_t user_id, int delta) override;
    void on_monster_bonus_change(std::size_t user_id, int delta) override;
    void on_level_change(std::size_t user_id, int delta) override;
    void on_card_receive(std::size_t user_id, std::size_t card_id) override;
    void on_card_loss(std::size_t user_id, std::size_t card_id) override;
    void on_monster_elimination(std::size_t user_id) override;  // which player killed monster
    void on_dice_roll(std::size_t user_id, unsigned res) override;
    void on_game_end(std::size_t winner_id) override;

    static std::shared_ptr<Scene> make_raylib_gameview() {
        return std::make_shared<RaylibGameView>();
    }

private:
    void setup_background();
    void setup_pause_menu();
    void setup_active_display_selector();
    void setup_hand();
    void draw_pause_menu();
    void draw_active_display_selector();
};

}  // namespace meow

#endif  // RAYLIB_GAME_VIEW_HPP_
