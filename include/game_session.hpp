#ifndef GAME_SESSION_HPP_
#define GAME_SESSION_HPP_

#include <memory>
#include <stdexcept>
#include <vector>
#include "shared_game_state.hpp"
#include "game_view.hpp"n
#include "virtual_machine.hpp"

namespace meow::model {

struct GameSession {
    friend struct VirtualMachine;

private:
    std::shared_ptr<GameView> game_view;
    std::size_t user_id{};

public:
    //  пока GameSession не дописан game будет public для удобства тестирования.
    SharedGameState shared_state;
    std::unique_ptr<GameState> current_state;

    explicit GameSession(const std::vector<std::size_t> &users)
        : shared_state(users), current_state(std::make_unique<InitState>(&shared_state)) {
        VirtualMachine::get_instance().set_game_session_reference(this);
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

}  // namespace meow::model

#endif
