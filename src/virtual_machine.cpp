#include "virtual_machine.hpp"
#include <optional>
#include <vector>
#include "game_session.hpp"
#include "model_command.hpp"
#include "model_player.hpp"
#include "shared_game_state.hpp"

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

    Player *player =
        game_session->shared_state.get_player_by_player_id(static_cast<std::size_t>(player_id));
    player->level_ += delta;

    if (!force) {
        player->level_ = std::max(1, std::min(9, player->level_));
    }
}

std::optional<int> VirtualMachine::execute(const std::vector<Command> &code) {
    assert(game_session != nullptr);
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
            case CommandType::IF: {
                assert(!st.empty());
                if (!st.top()) {
                    i++;
                }
                st.pop();
            } break;
            case CommandType::GOTO: {
                i = code[i].value;
            } break;
            case CommandType::LABEL: {
                continue;
            } break;
            case CommandType::DENY: {
                assert(!st.empty());
                bool res = !static_cast<bool>(st.top());
                st.pop();
                st.push(res);
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
                st.push(static_cast<int>(id == st.top()));
            } break;
            case CommandType::IS_USER: {
                size_t id = st.top();
                st.pop();
                for (const auto &player : game_session->shared_state.get_all_players()) {
                    if (player.obj_id == id) {
                        st.push(static_cast<int>(true));
                        continue;
                    }
                }
                st.push(static_cast<int>(false));
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