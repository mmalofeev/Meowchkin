#ifndef GAME_SESSION_HPP_
#define GAME_SESSION_HPP_

#include <memory>
#include <stdexcept>
#include <vector>
#include "noncopyable.hpp"
#include "game_view.hpp"
#include "shared_game_state.hpp"
#include "virtual_machine.hpp"

namespace meow::model {

struct GameSession : Noncopyable {
    friend struct VirtualMachine;

private:
    std::size_t user_id{};
public:
    //  пока GameSession не дописан game будет public для удобства тестирования.
    SharedGameState shared_state;
    std::unique_ptr<GameState> current_state;

    explicit GameSession(std::initializer_list<std::shared_ptr<GameView>> l) {
        Object::set_seed(0);
        VirtualMachine::get_instance().set_game_session_reference(this);
        
        for (auto observer : l) {
            shared_state.add_observer(observer);
        }
    }

    void init(const std::vector<std::size_t> &users) {
        shared_state.set_player_list(users);
        current_state = std::make_unique<InitState>(&shared_state);
    }


};

}  // namespace meow::model

#endif
