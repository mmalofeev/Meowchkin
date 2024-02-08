#include <cassert>
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

    enum class button { CREATE_LOBBY, CONNECT, QUIT, COUNT };
    const EnumArray<button, const char *> m_button_labels = {
        {button::CREATE_LOBBY, "Create lobby"},
        {button::CONNECT, "Connect to lobby"},
        {button::QUIT, "Quit"}};
    EnumArray<button, Rectangle> m_button_rects;
    EnumArray<button, bool> m_button_pressed;
    raylib::Texture m_background;

public:
    explicit MainMenu() {
        raylib::Image background_image("bin/imgs/kitiky.png");
        background_image.Resize(1920, 1080);
        m_background = raylib::Texture(background_image);
        for (std::size_t i = 0, h = 0; i < m_button_rects.size(); i++, h += button_height) {
            m_button_rects[i].height = button_height;
            m_button_rects[i].width = button_width;
            m_button_rects[i].x = 0;
            m_button_rects[i].y = h;
        }
        GuiLoadStyle("bin/gui_styles/meow.rgs");
        GuiSetFont(LoadFont("bin/fonts/mono.ttf"));
        GuiSetStyle(DEFAULT, TEXT_SIZE, 16);
    }

    void draw() override {
        assert(m_window);
        m_background.Draw();
        const int w = m_window->GetWidth();
        const int h = m_window->GetHeight();
        GuiStatusBar(Rectangle{w - 300.0f, h - 40.0f, 300, 40}, "status bar.....");
        for (std::size_t i = 0; i < m_button_rects.size(); i++) {
            m_button_rects[i].x = w / 2 - 150;
            m_button_rects[i].y = h / 2 + i * button_height;
            m_button_pressed[i] = static_cast<bool>(GuiButton(m_button_rects[i], m_button_labels[i]));
        }
        if (m_button_pressed[button::QUIT]) {
            m_running = false;
        }
        if (m_button_pressed[button::CREATE_LOBBY]) {
            assert(m_scene_manager);
            m_scene_manager->switch_scene(SceneType::GAME);
        }
    }
};

// NOLINTBEGIN cppcoreguidelines-avoid-non-const-global-variables
extern "C" BOOST_SYMBOL_EXPORT MainMenu main_menu;
MainMenu main_menu;
// NOLINTEND

}  // namespace meow
