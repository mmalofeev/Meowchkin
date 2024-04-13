#ifndef VIRTUAL_MACHINE_HPP_
#define VIRTUAL_MACHINE_HPP_
#include <optional>
#include <stack>
#include <vector>
#include "model_command.hpp"

namespace meow::model {

struct Game;

struct VirtualMachine {
private:
    Game *game;
    std::stack<int> st;
    VirtualMachine() = default;

    void increse_level(bool force);

public:
    void set_args(std::size_t player_id, std::size_t target_id);
    std::optional<int> execute(const std::vector<Command> &code);

    void set_game_reference(Game *_game) {
        game = _game;
    }

    static VirtualMachine &get_instance() {
        static VirtualMachine instance;
        return instance;
    }
};

}  // namespace meow::model

#endif