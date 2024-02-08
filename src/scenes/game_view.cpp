#include <cassert>
#include "enum_array.hpp"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raylib-cpp.hpp"
#include "scene.hpp"

namespace meow {
class GameView : public Scene {
private:
    static constexpr int card_width = 200;
    static constexpr int card_height = 330;
    static constexpr int card_offset = 50;
    static constexpr int button_width = 300;
    static constexpr int button_height = 40;
    static constexpr const char *background_image_path = "bin/imgs/kitik.png";
    const raylib::Color background_color = raylib::Color(77, 120, 204, 255);

    raylib::Texture m_background;
    raylib::Texture m_blur;
    std::vector<raylib::Rectangle> m_cards;
    std::vector<raylib::Vector2> source_positions;
    enum class PauseButton { CONTINUE, BACK_TO_LOBBY, QUIT, COUNT };  // TODO: settings
    EnumArray<PauseButton, const char *> m_pause_button_labels = {
        {PauseButton::CONTINUE, "Continue"},
        {PauseButton::BACK_TO_LOBBY, "Back to lobby"},
        {PauseButton::QUIT, "Quit"},
    };
    EnumArray<PauseButton, raylib::Rectangle> m_pause_button_rects;
    EnumArray<PauseButton, bool> m_pause_button_pressed;
    bool should_draw_pause = false;

public:
    explicit GameView() : m_cards(5), source_positions(m_cards.size()) {
        GuiLoadStyle("bin/gui_styles/meow.rgs");
        GuiSetFont(LoadFont("bin/fonts/mono.ttf"));
        GuiSetStyle(DEFAULT, TEXT_SIZE, 16);
    }

    void on_window_attach() override {
        const int offset = (m_window->GetWidth() - m_cards.size() * (card_width + card_offset)) / 2;
        for (int i = 0; i < m_cards.size(); i++) {
            source_positions[i].x = m_cards[i].x = offset + i * (card_width + card_offset);
            source_positions[i].y = m_cards[i].y = m_window->GetHeight() - (card_height + 80);
            m_cards[i].width = card_width;
            m_cards[i].height = card_height;
        }

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

    void draw() override {
        assert(m_window);
        m_background.Draw();
        bool card_held = false;
        for (std::size_t i = 0; i < m_cards.size(); i++) {
            auto &card = m_cards[i];
            if (!should_draw_pause && !card_held &&
                card.CheckCollision(raylib::Mouse::GetPosition())) {
                if (raylib::Mouse::IsButtonDown(MOUSE_BUTTON_LEFT)) {
                    card_held = true;
                    card.SetPosition(card.GetPosition() + raylib::Mouse::GetDelta());
                }
                card.DrawRounded(0.3f, 4, raylib::Color::Red());
            } else {
                card.DrawRounded(0.3f, 4, raylib::Color::Green());
            }
            if (!card_held) {
                card.SetPosition(Vector2Lerp(
                    card.GetPosition(), source_positions[i], m_window->GetFrameTime() * 4
                ));
            }
        }

        if (!should_draw_pause) {
            should_draw_pause = IsKeyPressed(KEY_ESCAPE);
        } else if (should_draw_pause) {
            m_blur.Draw();
            draw_pause_menu();
        }
    }

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
