#ifndef VIRTUAL_MACHINE_HPP_
#define VIRTUAL_MACHINE_HPP_
#include <memory>
#include <optional>
#include <stack>
#include <vector>
#include "abstract_observer.hpp"
#include "model_command.hpp"

namespace meow::model {

struct GameSession;

struct VirtualMachine {
private:
    GameSession *game_session;
    std::vector<std::shared_ptr<Observer>> observers;
    std::stack<int> st;
    VirtualMachine() = default;

    void increse_level(bool force);

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