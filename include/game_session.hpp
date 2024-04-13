#ifndef GAME_SESSION_HPP_
#define GAME_SESSION_HPP_

#include <vector>
#include "game.hpp"
// #include "game_view.hpp"
#include "message_types.hpp"
#include "virtual_machine.hpp"

namespace meow {

struct GameSession {
private:
    std::size_t user_id{};

public:
    //  пока GameSession не дописан game будет public для удобства тестирования.
    model::Game game;

    GameSession() {
        model::VirtualMachine::get_instance().set_game_reference(&game);
    }

    // GameView *observed;
    std::vector<const char *> cards_on_board;

    /*
    void notify_gameview(const network::Action &) {
        observed->on_new_card_on_board();
    }
    */
};

}  // namespace meow

#endif
