#include <cassert>
#include "raylib-cpp.hpp"
#include "scene.hpp"

namespace meow {
class GameView : public Scene {
private:
    static constexpr int card_width = 200;
    static constexpr int card_height = 330;
    static constexpr int card_offset = 50;

    raylib::Texture m_background;
    std::vector<raylib::Rectangle> m_cards;

public:
    explicit GameView() : m_cards(5) {
        raylib::Image background_image;
        try {
            background_image.Load("bin/imgs/kitik1.png");
        } catch (const raylib::RaylibException &) {
            background_image.Load("bin/imgs/kitik2.png");
        }
        background_image.Resize(1920, 1080);
        m_background = raylib::Texture(background_image);
        for (int i = 0; i < m_cards.size(); i++) {
            m_cards[i].x = 300 + i * (card_width + card_offset);
            m_cards[i].y = 1000 - card_height;
            m_cards[i].width = card_width;
            m_cards[i].height = card_height;
        }
    }

    void draw() override {
        assert(m_window);
        m_background.Draw();
        for (const auto &card : m_cards) {
            card.DrawRoundedLines(0.3f, 4, 3, GREEN);
            if (card.CheckCollision(GetMousePosition())) {
                card.Draw(RED);
            }
        }
    }

    void draw_pause_menu() {
    }
};

// NOLINTBEGIN cppcoreguidelines-avoid-non-const-global-variables
extern "C" BOOST_SYMBOL_EXPORT GameView game_view;
GameView game_view;
// NOLINTEND

}  // namespace meow
