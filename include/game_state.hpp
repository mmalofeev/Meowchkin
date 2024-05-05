#ifndef GAME_STATE_HPP_
#define GAME_STATE_HPP_
#include <cassert>
#include <memory>
#include <vector>

namespace meow::model {

struct SharedGameState;

enum class StateType { INIT, MANAGEMENT, BROWL, LOOKTROUBLE, POSTMANAGEMENT, END };

struct GameState {
protected:
    SharedGameState *shared_state;

public:
    const StateType type;

    GameState(SharedGameState *shared_state_, StateType type_)
        : shared_state(shared_state_), type(type_) {
        assert(shared_state != nullptr);
    }

    virtual ~GameState() = default;

    virtual std::unique_ptr<GameState>
    play_card(std::size_t user_id, std::size_t target_id, std::size_t card_obj_id) {
        return nullptr;
    }

    virtual std::unique_ptr<GameState> throw_card(std::size_t user_id, std::size_t card_obj_id) {
        return nullptr;
    }

    virtual std::unique_ptr<GameState> end_turn(std::size_t user_id) {
        return nullptr;
    }

    virtual std::unique_ptr<GameState> draw_card(std::size_t user_id) {
        return nullptr;
    }

    virtual std::unique_ptr<GameState> roll_dice(std::size_t user_id) {
        return nullptr;
    }
};

struct InitState : GameState {
private:
    std::vector<int> results;
    int count = 0;
    static constexpr std::size_t init_hand_size = 4;

public:
    InitState(SharedGameState *shared_state_);

    std::unique_ptr<GameState> roll_dice(std::size_t user_id) override;
};

struct ManagementState : GameState {
public:
    ManagementState(SharedGameState *shared_state_, StateType type_);

    std::unique_ptr<GameState>
    play_card(std::size_t user_id, std::size_t target_id, std::size_t card_obj_id) override;
    std::unique_ptr<GameState> throw_card(std::size_t user_id, std::size_t card_obj_id) override;
    std::unique_ptr<GameState> draw_card(std::size_t user_id) override;
};

struct PostManagementState : ManagementState {
public:
    PostManagementState(SharedGameState *shared_state_);

    std::unique_ptr<GameState> draw_card(std::size_t user_id) override {
        return nullptr;
    }

    std::unique_ptr<GameState> end_turn(std::size_t user_id) override;
};

/*
struct BrowlState : GameState {
private:
public:
};
*/

}  // namespace meow::model

#endif