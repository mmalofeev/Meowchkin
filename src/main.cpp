#include <memory>
#include "enum_array.hpp"
#include "plugin.hpp"
#include "raylib-cpp.hpp"
#include "scene.hpp"
#include "gui_dice_roller.hpp"

int main() {
    SetTraceLogLevel(LOG_WARNING);
    static constexpr int window_width = 1920;
    static constexpr int window_height = 1080;
    static constexpr const char *window_title = "meow";
    raylib::Window window(
        window_width, window_height, window_title,
        /*FLAG_WINDOW_RESIZABLE |*/ FLAG_MSAA_4X_HINT | FLAG_FULLSCREEN_MODE
    );

    // load scenes from 'plugins'
    const meow::EnumArray<meow::SceneType, std::pair<const char *, const char *>> plugin_names{
        {meow::SceneType::MAIN_MENU, {"mainmenu-scene", "main_menu"}},
        {meow::SceneType::GAME, {"game-scene", "game_view"}},
    };
    meow::Plugin<meow::Scene> main_menu(plugin_names[meow::SceneType::MAIN_MENU]);
    meow::Plugin<meow::Scene> game_view(plugin_names[meow::SceneType::GAME]);
    main_menu->attach_window(&window);
    game_view->attach_window(&window);

    SetExitKey(0);
    window.SetTargetFPS(60);

    auto scene_manager = std::make_unique<meow::SceneManager>();
    scene_manager->set_scene(meow::SceneType::MAIN_MENU, main_menu.get());
    scene_manager->set_scene(meow::SceneType::GAME, game_view.get());

    while (!window.ShouldClose() && scene_manager->active_scene()->running()) {
        // application response
        {
            if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_R)) {
                main_menu.reload(plugin_names[meow::SceneType::MAIN_MENU]);
                main_menu->attach_window(&window);
                scene_manager->set_scene(meow::SceneType::MAIN_MENU, main_menu.get());
                game_view.reload(plugin_names[meow::SceneType::GAME]);
                game_view->attach_window(&window);
                scene_manager->set_scene(meow::SceneType::GAME, game_view.get());
            }
        }
        // application render
        window.BeginDrawing();
        {
            // if (IsKeyPressed(KEY_R)) {
            //     meow::play_dice_animation(&window, -4, 8);
            // }
            window.ClearBackground(raylib::Color::RayWhite());
            scene_manager->draw_scene();
        }
        window.EndDrawing();
    }
}
