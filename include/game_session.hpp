#ifndef GAME_SESSION_HPP_
#define GAME_SESSION_HPP_

#include <memory>
#include <stdexcept>
#include <vector>
#include "abstract_observer.hpp"
#include "message_types.hpp"
#include "noncopyable.hpp"
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

    explicit GameSession(std::initializer_list<std::shared_ptr<Observer>> l) {
        Object::set_seed(0);
        VirtualMachine::get_instance().set_game_session_reference(this);

        for (auto observer : l) {
            VirtualMachine::get_instance().add_observer(observer);
        }
    }

    void init(const std::vector<std::size_t> &users) {
        shared_state.set_player_list(users);
        current_state = std::make_unique<InitState>(&shared_state);
    }

    bool handle_action(const network::Action &action) {
        using network::Action;
        switch (action.type) {
            case (Action::ActionType::EndTurn): {
                return static_cast<bool>(current_state->end_turn(action.sender_player));
            } break;
            case (Action::ActionType::RollDice): {
                return static_cast<bool>(current_state->roll_dice(action.sender_player));
            } break;
            case (Action::ActionType::PlayedCard): {
                return static_cast<bool>(current_state->play_card(
                    action.sender_player, action.target_player, action.card_id
                ));
            } break;
            case (Action::ActionType::DrawedCard): {
                return static_cast<bool>(current_state->draw_card(action.sender_player));
            } break;
            case (Action::ActionType::ThrewCard): {
                return static_cast<bool>(
                    current_state->throw_card(action.sender_player, action.card_id)
                );
            } break;
            case (Action::ActionType::Pass): {
                return static_cast<bool>(current_state->end_turn(action.sender_player));
            } break;
            default: {
                assert(false);
                return false;
            } break;
        }

        return false;
    }

    ~GameSession() {
        VirtualMachine::get_instance().release_observers();
    }
};

}  // namespace meow::model

#endif
