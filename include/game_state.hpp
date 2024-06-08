#ifndef GAME_STATE_HPP_
#define GAME_STATE_HPP_
#include <cassert>
#include <iterator>
#include <memory>
#include <set>
#include <vector>
#include "model_card.hpp"

namespace meow::model {

struct SharedGameState;

enum class StateType { INIT, MANAGEMENT, BRAWL, LOOKTROUBLE, SLIPAWAY, POSTMANAGEMENT, END };

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

    virtual std::unique_ptr<GameState> pass(std::size_t user_id) {
        return nullptr;
    }
};

struct InitState : GameState {
private:
    std::vector<int> results;
    int move_count = 0;
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

struct BrawlState : GameState {
    friend struct VirtualMachine;

private:
    std::vector<std::size_t> heroes;
    std::vector<std::unique_ptr<Card>> heroes_storage;
    std::vector<std::unique_ptr<MonsterCard>> monsters;
    // bool equal_power_condition = false;
    std::set<std::size_t> passed_players;
    int passed_heroes_count = 0;

public:
    BrawlState(
        SharedGameState *shared_state_,
        std::size_t player_id,
        std::unique_ptr<MonsterCard> monster
    );

    bool is_hero(std::size_t obj_id) const;
    bool is_monster(std::size_t obj_id) const;
    void add_hero(std::size_t player_id);
    void add_monster(std::unique_ptr<MonsterCard> monster);
    void update_monster(MonsterCard *monster);
    MonsterCard *get_monster(std::size_t obj_id);
    int get_monsters_power() const;
    int get_treasures_pool() const;
    int get_heroes_power() const;
    bool are_heroes_leading() const;

    std::unique_ptr<GameState>
    play_card(std::size_t user_id, std::size_t target_id, std::size_t card_obj_id) override;
    std::unique_ptr<GameState> pass(std::size_t user_id) override;
};

struct LookForTroubleState : GameState {
private:
    static constexpr std::size_t cards_to_deal = 2;

public:
    LookForTroubleState(SharedGameState *shared_state_);
    std::unique_ptr<GameState>
    play_card(std::size_t user_id, std::size_t target_id, std::size_t card_obj_id) override;
    std::unique_ptr<GameState> draw_card(std::size_t user_id) override;
};

struct EndState : GameState {
    EndState(SharedGameState *shared_state_);
};

struct SlipAwayState : GameState {
private:
    template <typename T>
    struct Transformer {
        using type = std::vector<T>::iterator;
    };

    std::vector<std::size_t> heroes;
    std::vector<std::unique_ptr<Card>> heroes_storage;
    std::vector<std::unique_ptr<MonsterCard>> monsters;
    std::vector<std::vector<std::unique_ptr<MonsterCard>>::iterator> monster_iters;
    int count_of_finished = 0;

public:
    SlipAwayState(
        SharedGameState *shared_state_,
        std::vector<std::size_t> heroes_,
        std::vector<std::unique_ptr<Card>> heroes_storage_,
        std::vector<std::unique_ptr<MonsterCard>> monsters_
    );
    std::unique_ptr<GameState> roll_dice(std::size_t user_id) override;
};

}  // namespace meow::model

#endif  // T -> std::vector<T>::iterator