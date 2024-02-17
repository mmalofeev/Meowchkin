#include <iostream>
#include "enum_array.hpp"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raylib-cpp.hpp"
#include "scene.hpp"

namespace meow {
class MainMenu : public Scene {
private:
    static constexpr int button_width = 300;
    static constexpr int button_height = 40;

    enum class Button { CREATE_LOBBY, CONNECT, QUIT, COUNT };
    const EnumArray<Button, const char *> m_button_labels = {
        {Button::CREATE_LOBBY, "Create lobby"},
        {Button::CONNECT, "Connect to lobby"},
        {Button::QUIT, "Quit"}};
    EnumArray<Button, Rectangle> m_button_rects;
    EnumArray<Button, bool> m_button_pressed;
    raylib::Texture m_background;

public:
    explicit MainMenu() {
        for (std::size_t i = 0, h = 0; i < m_button_rects.size(); i++, h += button_height) {
            m_button_rects[i].height = button_height;
            m_button_rects[i].width = button_width;
            m_button_rects[i].x = 0;
            m_button_rects[i].y = h;
        }
        // GuiLoadStyle("bin/gui_styles/meow.rgs");
        GuiSetFont(LoadFont("bin/fonts/mono.ttf"));
        GuiSetStyle(DEFAULT, TEXT_SIZE, 16);
    }

    void on_window_attach() override {
        if (m_window == nullptr) {
            throw std::runtime_error("invalid attached window ptr!");
        }
        raylib::Image background_image;
        try {
            background_image.Load("bin/imgs/kitiky.png");
        } catch (const raylib::RaylibException &) {
            background_image = raylib::Image::Color(
                m_window->GetWidth(), m_window->GetHeight(), raylib::Color::SkyBlue()
            );
        }
        background_image.Resize(m_window->GetWidth(), m_window->GetHeight());
        m_background = raylib::Texture(background_image);
        for (std::size_t i = 0; i < m_button_rects.size(); i++) {
            m_button_rects[i].x = m_window->GetWidth() / 2 - button_width / 2;
            m_button_rects[i].y = m_window->GetHeight() / 2 + i * button_height;
        }
    }

    void draw() override {
        if (m_window == nullptr) {
            throw std::runtime_error("invalid attached window ptr!");
        }
        m_background.Draw();
        const int w = m_window->GetWidth();
        const int h = m_window->GetHeight();
        static std::string status_bar_text = "status bar....";
        for (std::size_t i = 0; i < m_button_rects.size(); i++) {
            m_button_pressed[i] =
                static_cast<bool>(GuiButton(m_button_rects[i], m_button_labels[i]));
        }
        if (m_button_pressed[Button::QUIT]) {
            m_running = false;
        }
        if (m_button_pressed[Button::CREATE_LOBBY]) {
            if (m_scene_manager == nullptr) {
                throw std::runtime_error("invalid attached scene manager!");
            }
            m_scene_manager->switch_scene(SceneType::GAME);
        }
        static bool draw_textbox = false;
        if (draw_textbox) {
            GuiSetStyle(
                GuiControl::TEXTBOX, GuiControlProperty::TEXT_ALIGNMENT,
                GuiTextAlignment::TEXT_ALIGN_LEFT
            );
            static char a[100] = "Text Box";
            if (GuiTextBox(m_button_rects[Button::CONNECT], a, 100, draw_textbox)) {
                status_bar_text = a;
                draw_textbox = false;
            }
        } else if (m_button_pressed[Button::CONNECT]) {
            draw_textbox = true;
        }
        GuiStatusBar(Rectangle{w - 300.0f, h - 40.0f, 300, 40}, status_bar_text.c_str());
    }
};

// NOLINTBEGIN cppcoreguidelines-avoid-non-const-global-variables
extern "C" BOOST_SYMBOL_EXPORT MainMenu main_menu;
MainMenu main_menu;
// NOLINTEND

}  // namespace meow
