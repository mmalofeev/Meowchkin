#ifndef GAME_SESSION_HPP_
#define GAME_SESSION_HPP_

#include <memory>
#include <stdexcept>
#include <vector>
#include "game.hpp"
#include "game_view.hpp"
#include "virtual_machine.hpp"

namespace meow {

struct GameSession {
private:
    std::shared_ptr<GameView> game_view;
    std::size_t user_id{};

public:
    //  пока GameSession не дописан game будет public для удобства тестирования.
    model::Game game;

    explicit GameSession(std::shared_ptr<GameView> game_view) : game_view(game_view) {
        if (game_view == nullptr) {
            throw std::runtime_error("invalid game_view ptr passed to game_session!");
        }
        model::VirtualMachine::get_instance().set_game_reference(&game);
    }

    void reset_game_view(std::shared_ptr<GameView> game_view_) {
        game_view.reset();
        game_view = game_view_;
    }

    /*
    void notify_gameview(const network::Action &) {
        observed->on_new_card_on_board();
    }
    */
};

}  // namespace meow

#endif
