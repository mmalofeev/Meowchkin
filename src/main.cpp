#include <memory>
#include "enum_array.hpp"
#include "plugin.hpp"
#include "raylib-cpp.hpp"
#include "scene.hpp"

int main() {
    raylib::Window window(
        1920, 1080, "raylib-cpp [core] example - basic window",
        /*FLAG_WINDOW_RESIZABLE |*/ FLAG_MSAA_4X_HINT | FLAG_FULLSCREEN_MODE
    );

    // load plugins
    const meow::EnumArray<meow::SceneType, std::pair<const char *, const char *>> plugin_names{
        {meow::SceneType::MAIN_MENU, {"mainmenu-scene", "main_menu"}},
        {meow::SceneType::GAME, {"game-scene", "game_view"}},
    };
    meow::plugin<meow::Scene> main_menu(plugin_names[meow::SceneType::MAIN_MENU]);
    meow::plugin<meow::Scene> game_view(plugin_names[meow::SceneType::GAME]);
    main_menu->set_window(&window);
    game_view->set_window(&window);

    window.SetMinSize(1200, 800);
    SetExitKey(0);
    SetTargetFPS(60);

    auto scene_manager = std::make_unique<meow::SceneManager>();
    scene_manager->set_scene(meow::SceneType::MAIN_MENU, main_menu.get());
    scene_manager->set_scene(meow::SceneType::GAME, game_view.get());

    while (!window.ShouldClose() && scene_manager->active_scene()->running()) {
        // applicatoin response
        {
            if (IsKeyPressed(KEY_R)) {
                main_menu.reload(plugin_names[meow::SceneType::MAIN_MENU]);
                main_menu->set_window(&window);
                game_view.reload(plugin_names[meow::SceneType::GAME]);
                game_view->set_window(&window);
                scene_manager->set_scene(meow::SceneType::MAIN_MENU, main_menu.get());
            }
        }
        // application render
        window.BeginDrawing();
        {
            window.ClearBackground(RAYWHITE);
            window.DrawFPS();
            scene_manager->draw_scene();
        }
        window.EndDrawing();
    }
}
