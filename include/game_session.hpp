#ifndef GAME_SESSION_HPP_
#define GAME_SESSION_HPP_

// #include"game.hpp"
// #include"virtual_machine.hpp"
#include <memory>
#include <vector>
#include "message_types.hpp"
#include "game_view.hpp"

namespace meow {

struct GameSession {
private:
    // std::unique_ptr<model::Game> game_;
    // VirtualMachine vm;
public:
    GameView *observed;
    const std::size_t user_id = 0;  // TODO
    std::vector<const char *> cards_on_board;
    void notify_gameview(const network::Action &) {
        observed->on_new_card_on_board();
    }
};

}

#endif
