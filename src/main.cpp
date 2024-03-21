#include <memory>
#include "enum_array.hpp"
#include "gui_dice_roller.hpp"
#include "plugin.hpp"
#include "raylib-cpp.hpp"
#include "scene.hpp"

namespace meow {

class Application {
public:
    enum Type { CLIENT, SERVER };

private:
    static constexpr int window_width = 1920;
    static constexpr int window_height = 1080;
    static constexpr const char *window_title = "meow";

    inline static const meow::EnumArray<meow::SceneType, std::pair<const char *, const char *>>
        plugin_names{
            {meow::SceneType::MAIN_MENU, {"mainmenu-scene", "main_menu"}},
            {meow::SceneType::GAME, {"game-scene", "game_view"}},
        };

    raylib::Window m_window;
    meow::Plugin<meow::Scene> m_main_menu;
    meow::Plugin<meow::Scene> m_game_view;
    std::unique_ptr<meow::SceneManager> m_scene_manager;

public:
    explicit Application([[maybe_unused]] Type t)
        : m_window(
              window_width,
              window_height,
              window_title,
              FLAG_MSAA_4X_HINT | FLAG_FULLSCREEN_MODE
          ),
          m_main_menu(plugin_names[meow::SceneType::MAIN_MENU]),
          m_game_view(plugin_names[meow::SceneType::GAME]),
          m_scene_manager(std::make_unique<meow::SceneManager>()) {
        m_main_menu->attach_window(&m_window);
        m_game_view->attach_window(&m_window);

        m_scene_manager->set_scene(meow::SceneType::MAIN_MENU, m_main_menu.get());
        m_scene_manager->set_scene(meow::SceneType::GAME, m_game_view.get());

        SetExitKey(0);
        m_window.SetTargetFPS(60);
    }

    void run() {
        while (!m_window.ShouldClose() && m_scene_manager->active_scene()->running()) {
            response();
            render();
        }
    }

private:
    void response() {
        if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_R)) {
            m_main_menu.reload(plugin_names[meow::SceneType::MAIN_MENU]);
            m_main_menu->attach_window(&m_window);
            m_scene_manager->set_scene(meow::SceneType::MAIN_MENU, m_main_menu.get());
            m_game_view.reload(plugin_names[meow::SceneType::GAME]);
            m_game_view->attach_window(&m_window);
            m_scene_manager->set_scene(meow::SceneType::GAME, m_game_view.get());
        }
    }

    void render() {
        m_window.BeginDrawing();
        m_window.ClearBackground(raylib::Color::RayWhite());
        m_scene_manager->draw_scene();
        m_window.EndDrawing();
    }
};

}  // namespace meow

int main() {
    SetTraceLogLevel(LOG_WARNING);
    auto app = std::make_unique<meow::Application>(meow::Application::CLIENT);
    app->run();
}
