#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include "client.hpp"
#include "enum_array.hpp"
#include "gui_dice_roller.hpp"
#include "plugin.hpp"
#include "raylib-cpp.hpp"
#include "scene.hpp"
#include "server.hpp"

namespace meow {

namespace network {
constexpr const char *port_file = "port.txt";
constexpr std::size_t players_count = 2;
}  // namespace network

class Application {
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
    network::Client &m_client = network::Client::get_instance();

public:
    explicit Application()
        : m_window(
              window_width,
              window_height,
              window_title,
              FLAG_MSAA_4X_HINT | FLAG_FULLSCREEN_MODE
          ),
          m_main_menu(plugin_names[meow::SceneType::MAIN_MENU]),
          m_game_view(plugin_names[meow::SceneType::GAME]),
          m_scene_manager(std::make_unique<meow::SceneManager>()) {
        m_scene_manager->set_scene(meow::SceneType::MAIN_MENU, m_main_menu.get());
        m_scene_manager->set_scene(meow::SceneType::GAME, m_game_view.get());

        SetExitKey(0);
        m_window.SetTargetFPS(60);

        const std::string client_name = "bebrik" + std::to_string(meow::random_integer(1, 1'000));
        m_client.set_name_of_client(client_name);
        std::cout << "your name is " << client_name << '\n';

        std::ifstream f(network::port_file);
        std::string port;
        f >> port;
        f.close();
        m_client.connect(std::string("localhost:") + port);

        std::cout << "lobby:\n";
        for (const auto &info : m_client.get_players_info()) {
            std::cout << '\t' << info.name << '\n';
        }

        m_main_menu->attach_client(&m_client);
        m_game_view->attach_client(&m_client);

        m_main_menu->attach_window(&m_window);
        m_game_view->attach_window(&m_window);
    }

    // as client
    void run() {
        while (!m_window.ShouldClose() && m_scene_manager->active_scene()->running()) {
            response();
            render();
        }
    }

    static void run_server() {
        auto &server = meow::network::Server::get_instance();

        // running on single machine for now, so no hand typing port number
        // for clients hehehehhehehehehe
        std::ofstream f(network::port_file);
        f << server.get_port();
        f.close();

        server.start_listening(network::players_count);
        for (std::optional<meow::network::Action> action;; action = server.receive_action()) {
            if (action) {
                server.send_action(action->targeted_player, *action);
            }
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

int main(int argc, char **argv) {
    // for now, parsing argv and deducing server/client `application type`.
    // in future, will run server in separate thread once player created
    // lobby in main menu

    try {
        if (argc < 2) {
            throw std::runtime_error("not enough args. can't deduce application type:(");
        }

        if (std::string(argv[1]) == "host") {
            meow::Application::run_server();
        } else if (std::string(argv[1]) == "client") {
            SetTraceLogLevel(LOG_WARNING);
            auto app = std::make_unique<meow::Application>();
            app->run();
        } else {
            throw std::runtime_error("can't deduce application type:(");
        };

    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
