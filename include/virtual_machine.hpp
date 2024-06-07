#ifndef VIRTUAL_MACHINE_HPP_
#define VIRTUAL_MACHINE_HPP_
#include <memory>
#include <optional>
#include <stack>
#include <vector>
#include <cassert>
#include "abstract_observer.hpp"
#include "model_command.hpp"
#include "model_card.hpp"

namespace meow::model {

struct GameSession;

struct VirtualMachine {
private:
    GameSession *game_session;
    std::vector<std::shared_ptr<Observer>> observers;
    std::stack<int> st;
    VirtualMachine() = default;

    void increase_level(bool force);

    void increase_power();

public:
    template <typename... T>
    void set_args(const T &...args) {
        (st.push(static_cast<int>(args)), ...);
    }

    void add_observer(std::shared_ptr<Observer> observer) {
        observers.emplace_back(observer);
    }

    void release_observers() {
        observers.clear();
    }

    bool check_player_item_eligiblity(size_t player_id, ItemType itype, int quantity);

    void acquire_item(size_t player_id, ItemType itype, int quantity);

    std::size_t get_user_id_by_player_id(std::size_t player_id);

    std::optional<int> execute(const std::vector<Command> &code);

    void set_game_session_reference(GameSession *_game_session) {
        game_session = _game_session;
    }

    static VirtualMachine &get_instance() {
        thread_local static VirtualMachine instance;
        return instance;
    }

    std::vector<std::shared_ptr<Observer>> &get_observers() {
        return observers;
    }
};

}  // namespace meow::model

#endif