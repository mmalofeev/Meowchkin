#ifndef SCENE_HPP_
#define SCENE_HPP_

#include <boost/config.hpp>
#include <optional>
#include <string>
#include <string_view>
#include "client.hpp"
#include "enum_array.hpp"
#include "noncopyable.hpp"
#include "raylib-cpp.hpp"

namespace meow {

class SceneManager;

class BOOST_SYMBOL_VISIBLE Scene : Noncopyable {
    friend class SceneManager;

protected:
    raylib::Window *m_window = nullptr;
    SceneManager *m_scene_manager = nullptr;
    network::Client *m_client = nullptr;
    bool m_running = true;  // will be removed(?...)

    virtual void on_instances_attach() {
    }

public:
    Scene() = default;
    virtual void draw() = 0;

    // need to pass this to shared object
    void attach_instances(network::Client *client, raylib::Window *window) {
        m_client = client;
        m_window = window;
        on_instances_attach();
    }

    [[nodiscard]] bool running() const noexcept {
        return m_running;
    }
};

enum class SceneType { MAIN_MENU, GAME, COUNT };

class SceneManager : Noncopyable {
private:
    EnumArray<SceneType, Scene *> m_scenes;
    SceneType m_active_scene = SceneType::MAIN_MENU;
    std::optional<std::string> m_message_from_active_scene = std::nullopt;

public:
    SceneManager() = default;
    void draw_scene() const;
    void switch_scene(SceneType scene_type);
    void set_scene(SceneType scene_type, Scene *scene);
    [[nodiscard]] Scene *active_scene();

    void set_message(std::string_view msg) {
        m_message_from_active_scene = msg.data();
    }

    [[nodiscard]] std::optional<std::string> read_message() {
        if (m_message_from_active_scene) {
            return std::move(*m_message_from_active_scene);
        }
        return std::nullopt;
    }
};

}  // namespace meow

#endif  // SCENE_HPP_
