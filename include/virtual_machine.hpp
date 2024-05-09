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
    template <typename... T>
    void set_args(const T &...args) {
        (st.push(static_cast<int>(args)), ...);
    }

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