#ifndef GAME_SESSION_HPP_
#define GAME_SESSION_HPP_

#include <vector>
#include "game.hpp"
#include "game_view.hpp"
#include "virtual_machine.hpp"

namespace meow {

struct GameSession {
private:
    GameView &game_view;
    std::size_t user_id{};

public:
    //  пока GameSession не дописан game будет public для удобства тестирования.
    model::Game game;

    explicit GameSession(GameView &game_view) : game_view(game_view) {
        model::VirtualMachine::get_instance().set_game_reference(&game);
    }

    /*
    void notify_gameview(const network::Action &) {
        observed->on_new_card_on_board();
    }
    */
};

}  // namespace meow

#endif
