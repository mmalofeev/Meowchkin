#include <algorithm>
#include <boost/dll/alias.hpp>
#include <cassert>
#include <memory>
#include <optional>
#include <string_view>
#include "enum_array.hpp"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raylib-cpp.hpp"
#include "scene.hpp"

namespace meow {

struct GuiCard {
    static constexpr const int width = 230;
    static constexpr const int height = 350;
    raylib::Vector2 target_position;
    raylib::Rectangle border;
    raylib::Texture texture;
};

class GuiHand {
private:
    int m_card_count = 0;
    int m_card_gap = 50;
    bool m_card_held = false;
    std::vector<GuiCard> m_cards;

public:
    explicit GuiHand() : m_cards(m_card_count) {
    }

    void recalculate_card_rects(int window_width, int window_height, int new_card_number = -1) {
        const int offset =
            (window_width - m_card_count * (GuiCard::width + m_card_gap) + m_card_gap) / 2;
        for (std::size_t i = 0; i < m_cards.size(); i++) {
            if (new_card_number == i) {
                m_cards[i].border.x = window_width;
            }
            m_cards[i].target_position.x = offset + i * (GuiCard::width + m_card_gap);
            m_cards[i].border.y = m_cards[i].target_position.y =
                window_height - (GuiCard::height + 30);
            m_cards[i].border.width = GuiCard::width;
            m_cards[i].border.height = GuiCard::height;
        }
    }

    void add_card(int window_width, int window_height, std::string_view path_to_texture = "") {
        m_card_count++;
        raylib::Image img;
        try {
            img.Load(path_to_texture.data());
            img.Resize(GuiCard::width, GuiCard::height);
        } catch (const raylib::RaylibException &) {
            img = raylib::Image::Color(GuiCard::width, GuiCard::height, raylib::Color::Green());
        }
        m_cards.emplace_back(0, 0, raylib::Texture(img));
        m_card_gap -= 10;
        recalculate_card_rects(window_width, window_height, m_card_count - 1);
    }

    void remove_card(int window_width, int window_height) {
        m_card_count--;
        m_cards.pop_back();
        m_card_gap += 10;
        recalculate_card_rects(window_width, window_height);
    }

    void draw_cards(float frame_time) {
        bool holding_card = false;
        for (std::size_t i = 0; i < m_cards.size(); i++) {
            bool card_is_red = false;
            auto &card = m_cards[i];
            if (!holding_card && card.border.CheckCollision(raylib::Mouse::GetPosition())) {
                if (raylib::Mouse::IsButtonDown(MOUSE_BUTTON_LEFT)) {
                    holding_card = card_is_red = true;
                    card.border.SetPosition(card.border.GetPosition() + raylib::Mouse::GetDelta());
                }
            }
            if (!holding_card) {
                card.border.SetPosition(
                    Vector2Lerp(card.border.GetPosition(), card.target_position, frame_time * 4)
                );
            }
            if (card_is_red) {
                card.border.DrawRounded(0.1f, 4, raylib::Color(0xFF0000FF));
            } else {
                card.border.DrawRounded(0.1f, 4, raylib::Color(0x004400FF));
            }
            card.texture.Draw(card.border.GetPosition(), raylib::Color(255, 255, 255, 200));
            card.border.DrawRoundedLines(0.1f, 4, 3, raylib::Color::White());
        }
    }
};

class GameView : public Scene {
private:
    static constexpr const int button_width = 300;
    static constexpr const int button_height = 40;
    static constexpr const int board_width = 1200;
    static constexpr const int board_height = 700;
    static constexpr const char *background_image_path = "";
    static constexpr const char *board_image_path = "";
    const raylib::Color background_color = raylib::Color(77, 120, 204, 255);
    int m_cards_amount = 0;

    GuiHand m_player_hand;
    /* background */
    raylib::Texture m_background;
    raylib::Texture m_blur;
    /* board */
    raylib::Rectangle m_board_rect;
    raylib::Texture m_board_texture;
    /* pause menu */
    enum class PauseButton { CONTINUE, BACK_TO_LOBBY, QUIT, COUNT };  // TODO: settings
    EnumArray<PauseButton, const char *> m_pause_button_labels = {
        {PauseButton::CONTINUE, "Continue"},
        {PauseButton::BACK_TO_LOBBY, "Back to lobby"},
        {PauseButton::QUIT, "Quit"},
    };
    EnumArray<PauseButton, raylib::Rectangle> m_pause_button_rects;
    EnumArray<PauseButton, bool> m_pause_button_pressed;
    bool should_draw_pause = false;
    /* misc */
    /* ... */

public:
    void on_window_attach() override {
        m_player_hand.recalculate_card_rects(m_window->GetWidth(), m_window->GetHeight());
        const raylib::Vector2 board_offset = {(m_window->GetWidth() - board_width) / 2.0f, 10.0f};
        m_board_rect = raylib::Rectangle(board_offset.x, board_offset.y, board_width, board_height);
        raylib::Image board_image;
        try {
            board_image.Load(board_image_path);
            board_image.Resize(m_board_rect.GetSize().x, m_board_rect.GetSize().y);
        } catch (const raylib::RaylibException &) {
            board_image = raylib::Image::Color(
                m_board_rect.GetSize().x, m_board_rect.GetSize().y, raylib::Color::RayWhite()
            );
        }
        m_board_texture.Load(board_image);

        raylib::Image background_image;
        try {
            background_image.Load(background_image_path);
            background_image.Resize(m_window->GetWidth(), m_window->GetHeight());
        } catch (const raylib::RaylibException &) {
            background_image =
                raylib::Image::Color(m_window->GetWidth(), m_window->GetHeight(), background_color);
        }
        m_background.Load(background_image);

        for (std::size_t i = 0; i < m_pause_button_rects.size(); i++) {
            m_pause_button_rects[i].width = button_width;
            m_pause_button_rects[i].height = button_height;
            m_pause_button_rects[i].x = m_window->GetWidth() / 2 - button_width / 2;
            m_pause_button_rects[i].y = m_window->GetHeight() / 2 + i * button_height;
        }

        raylib::Image blur = raylib::Image::GradientRadial(
            m_window->GetWidth(), m_window->GetHeight(), 0, raylib::Color::Blank(), {0, 0, 0, 100}
        );
        m_blur.Load(blur);
    }

    explicit GameView() {
        GuiLoadStyle("bin/gui_styles/meow.rgs");
        GuiSetFont(LoadFont("bin/fonts/mono.ttf"));
        GuiSetStyle(DEFAULT, TEXT_SIZE, 16);
    }

    void draw() override {
        assert(m_window);
        m_background.Draw();
        // m_board_rect.Draw(RAYWHITE);
        m_board_texture.Draw(m_board_rect, m_board_rect.GetPosition());
        m_player_hand.draw_cards(m_window->GetFrameTime());

        if (GuiButton({0, 0, 40, 40}, "+") && m_cards_amount < 10) {
            m_player_hand.add_card(
                m_window->GetWidth(), m_window->GetHeight(), "bin/imgs/cards/original/door.png"
            );
            m_cards_amount++;
        }
        if (GuiButton({40, 0, 40, 40}, "-") && m_cards_amount > 0) {
            m_player_hand.remove_card(m_window->GetWidth(), m_window->GetHeight());
            m_cards_amount--;
        }
        GuiStatusBar(
            {0, (float)m_window->GetHeight() - 30, 100, 30}, TextFormat("%d", m_cards_amount)
        );

        if (!should_draw_pause) {
            should_draw_pause = IsKeyPressed(KEY_ESCAPE);
        } else if (should_draw_pause) {
            m_blur.Draw();
            draw_pause_menu();
        }
    }

private:
    void draw_pause_menu() {
        for (std::size_t i = 0; i < m_pause_button_labels.size(); i++) {
            m_pause_button_pressed[i] =
                GuiButton(m_pause_button_rects[i], m_pause_button_labels[i]);
        }
        if (m_pause_button_pressed[PauseButton::CONTINUE] || IsKeyPressed(KEY_ESCAPE)) {
            should_draw_pause = false;
        }
        if (m_pause_button_pressed[PauseButton::BACK_TO_LOBBY]) {
            assert(m_scene_manager);
            should_draw_pause = false;
            m_scene_manager->switch_scene(SceneType::MAIN_MENU);
        }
        if (m_pause_button_pressed[PauseButton::QUIT]) {
            should_draw_pause = false;
            m_running = false;
        }
    }
};

// NOLINTBEGIN cppcoreguidelines-avoid-non-const-global-variables
extern "C" BOOST_SYMBOL_EXPORT GameView game_view;
GameView game_view;
// NOLINTEND

}  // namespace meow
