#include <boost/dll/alias.hpp>
#include <cstring>
#include <iostream>
#include <string>
#include "enum_array.hpp"
#include "gui_dice_roller.hpp"
#define RAYGUI_IMPLEMENTATION
#include "paths_to_binaries.hpp"
#include "raygui.h"
#include "raylib-cpp.hpp"
#include "scene.hpp"

namespace meow {
class MainMenu : public Scene {
private:
    static constexpr int button_width = 300;
    static constexpr int button_height = 40;

    enum class Button { JOIN_LOBBY, CREATE_LOBBY, ENTER_NAME, QUIT, COUNT };
    static constexpr EnumArray<Button, const char *> m_button_labels = {
        {Button::JOIN_LOBBY, "Join lobby"},
        {Button::CREATE_LOBBY, "Create lobby"},
        {Button::ENTER_NAME, "Enter name"},
        {Button::QUIT, "Quit"}};
    EnumArray<Button, Rectangle> m_button_rects;
    EnumArray<Button, bool> m_button_pressed;
    raylib::Texture m_background;
    raylib::Texture m_loading_wheel_texture;
    raylib::Shader m_loading_wheel_shader;
    bool m_connecting = false;

public:
    explicit MainMenu() : m_loading_wheel_shader("", "bin/shaders/loading_wheel.fs") {
        for (std::size_t i = 0, h = 0; i < m_button_rects.size(); i++, h += button_height) {
            m_button_rects[i].height = button_height;
            m_button_rects[i].width = button_width;
            m_button_rects[i].x = 0;
            m_button_rects[i].y = h;
        }
        GuiLoadStyle(gui_style_path);
        GuiSetFont(LoadFont(gui_font_path));
        GuiSetStyle(DEFAULT, TEXT_SIZE, 16);
    }

    void on_instances_attach() override {
        if (m_window == nullptr) {
            throw std::runtime_error("invalid attached window ptr!");
        }

        raylib::Image background_image;
        try {
            background_image.Load(mainmenu_background_image_path);
        } catch (const raylib::RaylibException &) {
            background_image = raylib::Image::Color(
                m_window->GetWidth(), m_window->GetHeight(), raylib::Color::SkyBlue()
            );
        }
        background_image.Resize(m_window->GetWidth(), m_window->GetHeight());

        m_background = raylib::Texture(background_image);
        raylib::Image img = raylib::Image::Color(
            m_window->GetWidth(), m_window->GetHeight(), raylib::Color::Blank()
        );
        m_loading_wheel_texture.Load(img);

        for (std::size_t i = 0; i < m_button_rects.size(); i++) {
            m_button_rects[i].x = m_window->GetWidth() / 2 - button_width / 2;
            m_button_rects[i].y = m_window->GetHeight() / 2 + i * button_height;
        }
    }

    void draw() override {
        static const std::string default_nickname =
            ("bebrik" + std::to_string(meow::random_integer(1, 1'000)));
        static char nickname[100];
        static bool draw_textbox = false;
        static bool first_call = true;

        if (first_call) {
            first_call = false;
            std::strncpy(nickname, default_nickname.c_str(), default_nickname.length());
        }

        if (m_window == nullptr) {
            throw std::runtime_error("invalid attached window ptr!");
        }
        m_connecting = IsKeyDown(KEY_L);
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
        if (m_button_pressed[Button::JOIN_LOBBY]) {
            m_scene_manager->switch_scene(SceneType::GAME);
            m_scene_manager->set_message("join lobby:" + std::string(nickname));
        } else if (m_button_pressed[Button::CREATE_LOBBY]) {
            // } else
            // draw_textbox = true;
            m_scene_manager->switch_scene(SceneType::GAME);
            m_scene_manager->set_message("create lobby:" + std::string(nickname));
        }
        if (draw_textbox) {
            GuiSetStyle(
                GuiControl::TEXTBOX, GuiControlProperty::TEXT_ALIGNMENT,
                GuiTextAlignment::TEXT_ALIGN_LEFT
            );
            if (GuiTextBox(m_button_rects[Button::ENTER_NAME], nickname, 100, draw_textbox)) {
                status_bar_text = nickname;
                // nickname[0] = 0;
                draw_textbox = false;
            }
        } else if (m_button_pressed[Button::ENTER_NAME]) {
            draw_textbox = true;
        }

        const float t = m_window->GetTime();
        m_loading_wheel_shader.SetValue(
            m_loading_wheel_shader.GetLocation("iTime"), &t, SHADER_UNIFORM_FLOAT
        );
        if (m_connecting) {
            m_loading_wheel_shader.BeginMode();
            m_loading_wheel_texture.Draw();
            m_loading_wheel_shader.EndMode();
            status_bar_text = "connecting!!!";
        } else if (std::strlen(nickname) > 0) {
            status_bar_text = nickname;
        } else {
            status_bar_text = "status bar...";
        }
        GuiStatusBar(Rectangle{w - 300.0f, h - 40.0f, 300, 40}, status_bar_text.c_str());
    }

    static std::shared_ptr<MainMenu> make_mainmenu() {
        return std::make_shared<MainMenu>();
    }
};

BOOST_DLL_ALIAS(meow::MainMenu::make_mainmenu, make_mainmenu)

// NOLINTBEGIN cppcoreguidelines-avoid-non-const-global-variables
extern "C" BOOST_SYMBOL_EXPORT MainMenu main_menu;
MainMenu main_menu;
// NOLINTEND

}  // namespace meow
