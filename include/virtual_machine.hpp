#ifndef VIRTUAL_MACHINE_HPP_
#define VIRTUAL_MACHINE_HPP_
#include <optional>
#include <stack>
#include <vector>
#include "model_command.hpp"

namespace meow::model {

struct GameSession;

struct VirtualMachine {
private:
    GameSession *game_session;
    std::stack<int> st;
    VirtualMachine() = default;

    void increse_level(bool force);

public:
    void set_args(std::size_t player_id, std::size_t target_id);
    std::optional<int> execute(const std::vector<Command> &code);

    void set_game_session_reference(GameSession *_game_session) {
        game_session = _game_session;
    }

    static VirtualMachine &get_instance() {
        thread_local static VirtualMachine instance;
        return instance;
    }
};

}  // namespace meow::model

#endif