#ifndef MEOWCHKIN_VALIDATOR_HPP
#define MEOWCHKIN_VALIDATOR_HPP

#include "game_session.hpp"
#include "server.hpp"

namespace meow {
class Validator {
private:
    model::GameSession m_game_session{};

public:
    Validator() {
        m_game_session.init(network::Server::get_instance().get_clients_id());
    }

    void start_validating() {
        auto &server = network::Server::get_instance();

        for (std::optional<network::Action> action;; action = server.receive_action()) {
            if (!action) {
                continue;
            }

            bool validness = m_game_session.handle_action(action.value());
            if (validness) {
                server.send_action_to_all_clients(action.value());
                server.send_action_result(
                    action->sender_player, network::ActionResult(action->card_id, true, action->type)
                );
            } else {
                server.send_action_result(
                    action->sender_player, network::ActionResult(action->card_id, false, action->type)
                );
            }
        }
    }
};
}  // namespace meow

#endif  // MEOWCHKIN_VALIDATOR_HPP
