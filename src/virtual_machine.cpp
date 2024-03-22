#include "virtual_machine.hpp"
#include <optional>
#include <vector>
#include "model_command.hpp"
#include "model_player.hpp"

namespace meow::model {

void VirtualMachine::set_args(std::size_t player_id, std::size_t target_id) {
    st.push(static_cast<int>(player_id));
    st.push(static_cast<int>(target_id));
}

void VirtualMachine::increse_level(bool force = false) {
    int delta = st.top();
    st.pop();
    int player_id = st.top();
    st.pop();

    Player *player = game->get_player_by_player_id(static_cast<std::size_t>(player_id));
    player->level_ += delta;

    if (!force) {
        player->level_ = std::max(1, std::min(9, player->level_));
    }
}

std::optional<int> VirtualMachine::execute(const std::vector<Command> &code) {
    std::optional<int> res{};

    for (std::size_t i = 0; i < code.size(); i++) {
        switch (code[i].type) {
            case CommandType::STACK_LOAD: {
                st.push(code[i].value);
            } break;
            case CommandType::STACK_POP: {
                if (!st.empty()) {
                    st.pop();
                }
            } break;
            case CommandType::STACK_DOUBL: {
                st.push(st.top());
            } break;
            case CommandType::INCREASE_LEVEL: {
                increse_level();
            } break;
            case CommandType::INCREASE_LEVEL_FORCE: {
                increse_level(true);
            } break;
            case CommandType::IS_DESK: {
                int id = st.top();
                st.pop();
                st.push(static_cast<int>(static_cast<std::size_t>(id) == game->get_desk_id()));
            } break;
            case CommandType::RETURN: {
                res = st.top();
                goto end_label;
            } break;
        }
    }
end_label:
    while (!st.empty()) {
        st.pop();
    }
    return res;
}

}  // namespace meow::model