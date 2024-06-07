#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include "bd_observer.hpp"
#include "client.hpp"
#include "enum_array.hpp"
#include "game_session.hpp"
// #include "paths_to_binaries.hpp"
#include "game_view.hpp"
#include "model_card_manager.hpp"
#include "plugin.hpp"
#include "raylib.h"
#include "scene.hpp"
#include "server.hpp"
#include "validator.hpp"

namespace meow {

namespace network {
constexpr const char *port_file = "port.txt";
constexpr std::size_t players_count = 2;
}  // namespace network

class Application {
private:
    static constexpr unsigned window_width = 1920;
    static constexpr unsigned window_height = 1080;
    static constexpr const char *window_title = "meow";

    static constexpr EnumArray<SceneType, std::pair<const char *, const char *>> plugin_names{
        {SceneType::MAIN_MENU, {"mainmenu-scene", "make_mainmenu"}},
        {SceneType::GAME, {"gameview-scene", "make_gameview"}},
    };

    raylib::Window m_window;
    raylib::AudioDevice m_audio_device;
    raylib::Music m_music;
    // raylib::Shader m_loading_wheel_shader;

    using maker_scene_t = std::shared_ptr<Scene> (*)();
    Plugin<maker_scene_t> m_gameview_maker;
    Plugin<maker_scene_t> m_mainmenu_maker;
    std::shared_ptr<Scene> m_gameview;
    std::shared_ptr<Scene> m_mainmenu;
    std::unique_ptr<SceneManager> m_scene_manager;

    network::Client &m_client = network::Client::get_instance();
    std::string m_client_name;
    model::GameSession m_game_session;

    bool m_connected = false;

public:
    Application()
        : m_window(
              window_width,
              window_height,
              window_title,
              FLAG_MSAA_4X_HINT | FLAG_FULLSCREEN_MODE
          ),
          // m_loading_wheel_shader("", path_to_loading_wheel_shader),
          m_mainmenu_maker(plugin_names[SceneType::MAIN_MENU]),
          m_mainmenu((*m_mainmenu_maker)()),
          m_gameview_maker(plugin_names[SceneType::GAME]),
          m_gameview((*m_gameview_maker)()),
          m_scene_manager(std::make_unique<SceneManager>()),
          m_game_session(
              {std::dynamic_pointer_cast<GameView>(m_gameview)}
               // std::make_shared<StatisticObserver>()}
          ),
          m_music("bin/music/witcher-gwent.mp3") {
        std::dynamic_pointer_cast<GameView>(m_gameview)->card_manager =
            &CardManager::get_instance();
        std::dynamic_pointer_cast<GameView>(m_gameview)->game_session = &m_game_session;
        m_scene_manager->set_scene(SceneType::MAIN_MENU, m_mainmenu.get());
        m_scene_manager->set_scene(SceneType::GAME, m_gameview.get());

        SetExitKey(0);
        m_window.SetTargetFPS(60);
        m_mainmenu->attach_instances(&m_client, &m_window);
        // m_music.SetLooping(true);
        // m_music.Play();
    }

    void run() {
        while (!m_window.ShouldClose() && m_scene_manager->active_scene()->running()) {
            if (auto action = m_client.receive_action(); action) {
                m_game_session.handle_action(*action);
            }
            response();
            render();
        }
    }

private:
    static void run_server() {
        auto &server = network::Server::get_instance();

        // running on single machine for now, so no hand typing port number
        // for clients hehehehhehehehehe
        std::ofstream f(network::port_file);
        f << server.get_port();
        f.close();

        server.start_listening(network::players_count);
        Validator m_validator;
        m_validator.start_validating();
    }

    void join_lobby() {
        m_client.set_name_of_client(m_client_name);
        std::cout << "your name is " << m_client_name << '\n';

        std::ifstream f(network::port_file);
        std::string port;
        f >> port;
        f.close();
        std::cout << "joining to port " << port << '\n';
        m_client.connect(std::string("localhost:") + port);

        const auto &players = m_client.get_players_info();

        std::cout << "lobby:\n";
        for (const auto &info : players) {
            std::cout << '\t' << info.name << '\n';
        }

        std::vector<std::size_t> users(players.size());
        std::transform(
            players.cbegin(), players.cend(), users.begin(),
            [](const network::PlayerInfo &player) { return player.id; }
        );
        m_game_session.init(users);

        std::dynamic_pointer_cast<GameView>(m_gameview)->game_session = &m_game_session;
        m_gameview->attach_instances(&m_client, &m_window);
    }

    void response() {
        if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_R)) {
            m_mainmenu_maker.reload(plugin_names[SceneType::MAIN_MENU]);
            m_gameview_maker.reload(plugin_names[SceneType::GAME]);

            m_mainmenu.reset();
            m_mainmenu = (*m_mainmenu_maker)();
            m_gameview.reset();
            m_gameview = (*m_gameview_maker)();
            // m_game_session.reset_game_view(std::dynamic_pointer_cast<GameView>(m_gameview));

            m_mainmenu->attach_instances(&m_client, &m_window);
            m_gameview->attach_instances(&m_client, &m_window);

            m_scene_manager->set_scene(SceneType::MAIN_MENU, m_mainmenu.get());
            m_scene_manager->set_scene(SceneType::GAME, m_gameview.get());
        }

        if (!m_connected) {
            if (auto msg = m_scene_manager->read_message(); msg) {
                if (msg->find("join lobby") != std::string::npos) {
                    m_client_name = msg->substr(msg->find(':') + 1);
                    join_lobby();
                    m_connected = true;
                } else if (msg->find("create lobby") != std::string::npos) {
                    m_client_name = msg->substr(msg->find(':') + 1);
                    std::thread(Application::run_server).detach();
                    network::Server::get_instance().wait_for_listening();
                    join_lobby();
                    m_connected = true;
                }
            }
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
    try {
        SetTraceLogLevel(LOG_WARNING);
        auto app = std::make_unique<meow::Application>();
        app->run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "wtf\n";
        return 1;
    }
}
