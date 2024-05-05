#ifndef GAME_SESSION_HPP_
#define GAME_SESSION_HPP_

#include <vector>
#include "shared_game_state.hpp"
#include "virtual_machine.hpp"
// #include "game_view.hpp"
#include "message_types.hpp"

namespace meow::model {

struct GameSession {
    friend struct VirtualMachine;

private:
    std::size_t user_id{};

public:
    //  пока GameSession не дописан game будет public для удобства тестирования.
    SharedGameState shared_state;
    std::unique_ptr<GameState> current_state;

    GameSession(const std::vector<std::size_t> &users)
        : shared_state(users), current_state(std::make_unique<InitState>(&shared_state)) {
        VirtualMachine::get_instance().set_game_session_reference(this);
    }

    // GameView *observed;
    // std::vector<const char *> cards_on_board;

    /*
    void notify_gameview(const network::Action &) {
        observed->on_new_card_on_board();
    }
    */
};

}  // namespace meow::model

#endif
