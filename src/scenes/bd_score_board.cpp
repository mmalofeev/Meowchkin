#include <algorithm>
#include <boost/dll/alias.hpp>
#include <sstream>
#include "Functions.hpp"
#include "bd_observer.hpp"
#include "bd_scoreboard_scene.hpp"
#include "paths_to_binaries.hpp"
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

namespace meow {

class ScoreBoard : public ScoreBoardBase {
private:
    static constexpr int button_width = 400;
    static constexpr int button_height = 80;

    static constexpr float ratio = 23.0f / 35;
    static constexpr float card_height = 800.0f;
    static constexpr float card_width = ratio * card_height;

    raylib::Texture m_background;
    enum class Button { BACK_TO_MAINMENU, COUNT };
    EnumArray<Button, raylib::Rectangle> m_button_rects;
    EnumArray<Button, bool> m_button_pressed;
    EnumArray<Button, const char *> m_button_labels = {
        {Button::BACK_TO_MAINMENU, "Back to main menu"}
    };

    // StatisticObserver m_stats;

    struct Card {
        raylib::Texture texture;
        UsageItemInfo usage;
    };

    std::vector<Card> m_cards;
    std::vector<Card>::iterator m_cards_iter = m_cards.end();

    using ScoreBoardBase::card_manager;

public:
    explicit ScoreBoard() {
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
        m_background = raylib::Texture("bin/imgs/wooden-background.png");
        m_button_rects[Button::BACK_TO_MAINMENU] = raylib::Rectangle{
            0, m_window->GetHeight() - (float)button_height, button_width, button_height
        };

        // auto v = m_stats.get_frequency_of_usage_items();
        // std::vector<UsageItemInfo> v{{2, 3}, {1, 0}, {4, 4}};
        StatisticObserver so;
        auto v1 = so.card_id_to_file_path();
        auto v2 = so.get_frequency_of_usage_items();
        for (std::size_t i = 0; i < std::min(v1.size(), v2.size()); ++i) {
            m_cards.emplace_back(
                raylib::Texture(
                    raylib::Image(v1[v2[i].card_id]).Resize(card_width, card_height).Mipmaps()
                ),
                v2[i]
            );
        }
        m_cards_iter = m_cards.begin();
        std::sort(m_cards.begin(), m_cards.end(), [](const auto &lhs, const auto &rhs) {
            return lhs.usage.frequency < rhs.usage.frequency;
        });
    }

    void draw() override {
        if (m_window == nullptr) {
            throw std::runtime_error("invalid attached window ptr!");
        }
        m_background.Draw();
        for (std::size_t i = 0; i < m_button_pressed.size(); ++i) {
            m_button_pressed[i] = GuiButton(m_button_rects[i], m_button_labels[i]);
        }

        if (m_button_pressed[Button::BACK_TO_MAINMENU]) {
            m_scene_manager->switch_scene(SceneType::MAIN_MENU);
        }

        if (IsKeyPressed(KEY_RIGHT)) {
            ++m_cards_iter;
            if (m_cards_iter == m_cards.end()) {
                m_cards_iter = m_cards.begin();
            }
        } else if (IsKeyPressed(KEY_LEFT)) {
            if (m_cards_iter == m_cards.begin()) {
                m_cards_iter = m_cards.end() - 1;
            } else {
                --m_cards_iter;
            }
        }

        const float x0 = m_window->GetWidth() / 2.0f - card_width / 2;
        const float y0 = m_window->GetHeight() / 2.0f - card_height / 2;
        m_cards_iter->texture.Draw(raylib::Vector2{x0, y0});
        const std::string text =
            (std::ostringstream{} << "Count of usage: " << m_cards_iter->usage.frequency).str();
        const int szx = raylib::MeasureText(text, 60);
        raylib::DrawText(
            text, x0 + card_width / 2 - szx / 2.0f, y0 + card_height + 60, 60, raylib::Color::RayWhite()
        );
    }

    static std::shared_ptr<Scene> make_scoreboard() {
        return std::make_shared<ScoreBoard>();
    }
};

}  // namespace meow

BOOST_DLL_ALIAS(meow::ScoreBoard::make_scoreboard, make_scoreboard)
