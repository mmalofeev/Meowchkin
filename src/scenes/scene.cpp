#include "scene.hpp"

void meow::SceneManager::draw_scene() const {
    m_scenes[m_active_scene]->draw();
}

void meow::SceneManager::switch_scene(SceneType scene_type) {
    m_active_scene = scene_type;
    m_scenes[scene_type]->m_scene_manager = this;
}

void meow::SceneManager::set_scene(SceneType scene_type, Scene *scene) {
    m_scenes[scene_type] = scene;
    m_scenes[scene_type]->m_scene_manager = this;
}

[[nodiscard]] meow::Scene *meow::SceneManager::active_scene() {
    return m_scenes[m_active_scene];
}
