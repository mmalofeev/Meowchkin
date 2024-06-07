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
        std::unique_ptr<GameState> next_state;
        switch (action.type) {
            case (Action::ActionType::EndTurn): {
                next_state = current_state->end_turn(action.sender_player);
            } break;
            case (Action::ActionType::RollDice): {
                next_state = current_state->roll_dice(action.sender_player);
            } break;
            case (Action::ActionType::PlayedCard): {
                next_state = current_state->play_card(
                    action.sender_player, action.target_player, action.card_id
                );
            } break;
            case (Action::ActionType::DrawedCard): {
                next_state = current_state->draw_card(action.sender_player);
            } break;
            case (Action::ActionType::ThrewCard): {
                next_state = current_state->throw_card(action.sender_player, action.card_id);
            } break;
            case (Action::ActionType::Pass): {
                next_state = current_state->pass(action.sender_player);
            } break;
            default: {
                assert(false);
            } break;
        }

        if (!next_state) {
            return false;
        }
        if (current_state.get() != next_state.get()) {
            current_state = std::move(next_state);
        } else {
            next_state.release();
        }

        return true;
    }

    std::size_t get_player_id_by_user_id(std::size_t user_id) {
        return shared_state.get_player_by_user_id(user_id)->obj_id;
    }

    ~GameSession() {
        VirtualMachine::get_instance().release_observers();
    }
};

}  // namespace meow::model

#endif
