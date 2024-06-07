#ifndef PLAYER_HPP_
#define PLAYER_HPP_
#include <algorithm>
#include <memory>
#include <optional>
#include <set>
#include <vector>
#include "model_card.hpp"
#include "model_object.hpp"

namespace meow::model {

struct Player : Object {
    friend struct VirtualMachine;

private:
    int level = 1;
    int power_buff = 0;
    int slip_away_buff = 0;

    int unused_arms = 2;
    int unused_helmet = 1;
    int unused_breastplate = 1;
    int unused_boots = 1;
    int cards_after_turn_balance = 5;
    static constexpr std::size_t max_cards_in_hand = 10;

    // оставил на потом
    // std::optional<Race> race_;
    // std::optional<Class> class_;

    std::vector<std::unique_ptr<Card>> hand;
    std::vector<std::unique_ptr<Card>> storage;

public:
    const std::size_t user_id;

    explicit Player(std::size_t user_id) : user_id(user_id) {
    }

    int get_power() const {
        return std::max(level + power_buff, 0);
    }

    int get_level() const {
        return level;
    }

    void increase_level(int delta, bool force = false);
    void increase_power(int delta);

    void add_card_to_storage(std::unique_ptr<Card> card) {
        storage.emplace_back(std::move(card));
    }

    void add_card_to_hand(std::unique_ptr<Card> card);

    std::unique_ptr<Card> drop_card_from_hand_by_id(std::size_t obj_id);

    std::unique_ptr<Card> drop_card_from_storage_by_id(std::size_t obj_id);

    const Card *get_card_from_hand_by_id(std::size_t obj_id) const;

    const std::vector<std::unique_ptr<Card>> &get_hand() const {
        return hand;
    }

    bool play_card_by_id(std::size_t card_obj_id, std::size_t target_id);
};

}  // namespace meow::model

#endif