#include <filesystem>
#include <random>
#include "enum_array.hpp"
#include "gui_board.hpp"
#include "gui_card_span.hpp"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raylib-cpp.hpp"
#include "scene.hpp"

namespace meow {

class GameView : public Scene {
private:
    static constexpr int button_width = 300;
    static constexpr int button_height = 40;
    static constexpr const char *background_image_path = "";
    static constexpr const char *board_image_path = "bin/imgs/kitik3.png";
    const raylib::Color background_color = raylib::Color(77, 120, 204, 255);

    GuiBoard m_board;
    GuiCardSpan m_player_hand;

    /* background */
    raylib::Texture m_background;
    raylib::Texture m_blur;

    /* pause menu */
    enum class PauseButton { CONTINUE, BACK_TO_LOBBY, QUIT, COUNT };  // TODO: settings
    EnumArray<PauseButton, const char *> m_pause_button_labels = {
        {PauseButton::CONTINUE, "Continue"},
        {PauseButton::BACK_TO_LOBBY, "Back to lobby"},
        {PauseButton::QUIT, "Quit"},
    };
    EnumArray<PauseButton, raylib::Rectangle> m_pause_button_rects;
    EnumArray<PauseButton, bool> m_pause_button_pressed;
    bool m_should_draw_pause = false;

    /* misc */
    std::vector<std::filesystem::path> m_card_image_paths;

protected:
    void on_window_attach() override {
        setup_background();
        setup_pause_menu();
        setup_hand();
        m_board.setup(m_window, &m_player_hand);
    }

public:
    explicit GameView() {
        GuiLoadStyle("bin/gui_styles/meow.rgs");
        GuiSetFont(LoadFont("bin/fonts/mono.ttf"));
        GuiSetStyle(DEFAULT, TEXT_SIZE, 16);
        for (const auto &entry : std::filesystem::directory_iterator("bin/imgs/cards/original/")) {
            if (entry.path().extension() == ".png") {
                m_card_image_paths.emplace_back(entry);
            }
        }
    }

    void draw() override {
        if (!m_window) {
            throw std::runtime_error("invalid attached window!");
        }
        m_background.Draw();

        m_board.draw(m_window->GetFrameTime(), m_should_draw_pause);
        m_player_hand.draw_cards(m_window->GetFrameTime(), m_should_draw_pause);

        if (!m_should_draw_pause && GuiButton({0, 0, 40, 40}, "-") && m_player_hand.card_count() > 0) {
            m_player_hand.remove_card();
        }
        if (!m_should_draw_pause && GuiButton({40, 0, 40, 40}, "+") && m_player_hand.card_count() < 10) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(0, m_card_image_paths.size() - 1);
            int random_index = distrib(gen);
            m_player_hand.add_card(m_card_image_paths[random_index].c_str());
        }
        GuiStatusBar({0, (float)m_window->GetHeight() - 30, 100, 30}, "status bar....");

        if (!m_should_draw_pause) {
            m_should_draw_pause = IsKeyPressed(KEY_ESCAPE);
        } else if (m_should_draw_pause) {
            m_blur.Draw();
            draw_pause_menu();
        }
    }

private:
    void setup_background() {
        raylib::Image background_image;
        try {
            background_image.Load(background_image_path);
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

    void setup_pause_menu() {
        for (std::size_t i = 0; i < m_pause_button_rects.size(); i++) {
            m_pause_button_rects[i].width = button_width;
            m_pause_button_rects[i].height = button_height;
            m_pause_button_rects[i].x = m_window->GetWidth() / 2 - button_width / 2;
            m_pause_button_rects[i].y = m_window->GetHeight() / 2 + i * button_height;
        }
    }

    void setup_hand() {
        m_player_hand.set_window(m_window);
        m_player_hand.set_span_borders({m_window->GetPosition(), m_window->GetSize()});
        for (int i = 0; i < 5; i++) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distrib(0, m_card_image_paths.size() - 1);
            int random_index = distrib(gen);
            m_player_hand.add_card(m_card_image_paths[random_index].c_str());
        }
    }

    void draw_pause_menu() {
        for (std::size_t i = 0; i < m_pause_button_labels.size(); i++) {
            m_pause_button_pressed[i] =
                GuiButton(m_pause_button_rects[i], m_pause_button_labels[i]);
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
};

// NOLINTBEGIN cppcoreguidelines-avoid-non-const-global-variables
extern "C" BOOST_SYMBOL_EXPORT GameView game_view;
GameView game_view;
// NOLINTEND

}  // namespace meow
