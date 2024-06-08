#ifndef APPLICATION_HPP_
#define APPLICATION_HPP_

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include "bd_observer.hpp"
#include "client.hpp"
#include "enum_array.hpp"
#include "game_session.hpp"
#include "game_view.hpp"
#include "gui_card.hpp"
#include "model_card_manager.hpp"
#include "paths_to_binaries.hpp"
#include "plugin.hpp"
#include "raylib.h"
#include "scene.hpp"
#include "server.hpp"
#include "validator.hpp"

namespace meow {

namespace network {
constexpr std::size_t players_count = 2;
}  // namespace network

class Application {
private:
    static constexpr int window_width = 1920;
    static constexpr int window_height = 1080;
    static constexpr const char *window_title = "meow";

    static constexpr EnumArray<SceneType, std::pair<const char *, const char *>> plugin_names{
        {SceneType::MAIN_MENU, {"mainmenu-scene", "make_mainmenu"}},
        {SceneType::GAME, {"gameview-scene", "make_gameview"}},
        {SceneType::SCORE_BOARD, {"scoreboard-scene", "make_scoreboard"}},
    };

    raylib::Window m_window;
    raylib::AudioDevice m_audio_device;
    raylib::Music m_music;

    using maker_scene_t = std::shared_ptr<Scene> (*)();
    Plugin<maker_scene_t> m_gameview_maker;
    Plugin<maker_scene_t> m_mainmenu_maker;
    Plugin<maker_scene_t> m_scoreboard_maker;
    std::shared_ptr<Scene> m_gameview;
    std::shared_ptr<Scene> m_mainmenu;
    std::shared_ptr<Scene> m_scoreboard;
    std::unique_ptr<SceneManager> m_scene_manager;

    network::Client &m_client = network::Client::get_instance();
    std::string m_client_name;
    std::string m_port_str;
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
          m_gameview_maker(plugin_names[SceneType::GAME]),
          m_mainmenu_maker(plugin_names[SceneType::MAIN_MENU]),
          m_scoreboard_maker(plugin_names[SceneType::SCORE_BOARD]),
          m_gameview((*m_gameview_maker)()),
          m_mainmenu((*m_mainmenu_maker)()),
          m_scoreboard((*m_scoreboard_maker)()),
          m_scene_manager(std::make_unique<SceneManager>()),
          m_game_session(
              {std::dynamic_pointer_cast<GameView>(m_gameview),
               std::make_shared<StatisticObserver>()}
          ),
          m_music(path_to_mainmenu_background_music) {
#ifdef NDEBUG
        std::freopen("log.txt", "w", stdout);
#endif

        std::dynamic_pointer_cast<GameView>(m_gameview)->card_manager =
            &CardManager::get_instance();
        std::dynamic_pointer_cast<GameView>(m_gameview)->game_session = &m_game_session;

        m_scene_manager->set_scene(SceneType::MAIN_MENU, m_mainmenu.get());
        m_scene_manager->set_scene(SceneType::GAME, m_gameview.get());
        m_scene_manager->set_scene(SceneType::SCORE_BOARD, m_scoreboard.get());

        SetExitKey(0);
        m_window.SetTargetFPS(60);
        m_mainmenu->attach_instances(&m_client, &m_window);
        m_music.SetLooping(true);
        m_music.Play();
        m_scoreboard->attach_instances(&m_client, &m_window);
    }

    void run() {
        while (!m_window.ShouldClose() && m_scene_manager->active_scene()->running()) {
            if (auto action = m_client.receive_action(); action) {
                m_game_session.handle_action(*action);
            }
            response();
            render();
        }
        StatisticObserver so;
        so.display_table();
    }

private:
    static void run_server() {
        auto &server = network::Server::get_instance();

        server.start_listening(network::players_count);
        Validator m_validator;
        m_validator.start_validating();
    }

    void join_lobby(std::string port) {
        dbg;
        m_client.set_name_of_client(m_client_name);
        std::cerr << "your name is " << m_client_name << '\n';
        std::cerr << "joining to port " << port << std::endl;
        m_client.connect(std::string("localhost:") + port);

        dbg;
        const auto &players = m_client.get_players_info();

        dbg;
        std::cout << "lobby:\n";
        for (const auto &info : players) {
            std::cout << '\t' << info.name << '\n';
        }

        dbg;
        std::vector<std::size_t> users(players.size());
        std::transform(
            players.cbegin(), players.cend(), users.begin(),
            [](const network::PlayerInfo &player) { return player.id; }
        );
        dbg;
        m_game_session.init(users);

        dbg;
        std::dynamic_pointer_cast<GameView>(m_gameview)->game_session = &m_game_session;
        dbg;
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
            m_music.Update();
            if (auto msg = m_scene_manager->read_message(); msg) {
                if (msg->find("join lobby") != std::string::npos) {
                    std::string buf = msg->substr(msg->find(':') + 1);
                    m_client_name = std::string(buf.begin(), buf.begin() + buf.find(':'));
                    buf = buf.substr(buf.find(':') + 1);
                    join_lobby(buf);
                    m_connected = true;
                } else if (msg->find("create lobby") != std::string::npos) {
                    m_client_name = msg->substr(msg->find(':') + 1);
                    std::thread(Application::run_server).detach();
                    network::Server::get_instance().wait_for_listening();
                    join_lobby(network::Server::get_instance().get_port());
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

#endif  // APPLICATION_HPP_
