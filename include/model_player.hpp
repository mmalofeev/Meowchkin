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
    int level_ = 1;
    int power_buff = 0;
    int slip_away_buff = 0;

    int unused_arms = 2;
    int unused_smut = 1;
    int unused_breastplate = 1;
    int cards_after_turn_balance = 5;
    const std::size_t max_cards_in_hand = 10;

    // оставил на потом
    // std::optional<Race> race_;
    // std::optional<Class> class_;

    std::vector<std::unique_ptr<Card>> hand;
    std::set<std::unique_ptr<Card>> storage;

public:
    const std::size_t user_id;

    Player(std::size_t user_id) : user_id(user_id) {
    }

    int power() const {
        return std::max(level_ + power_buff, 0);
    }

    int level() const {
        return level_;
    }

    void add_card_to_storage(std::unique_ptr<Card> card) {
        storage.insert(std::move(card));
    }

    void add_card_to_hand(std::unique_ptr<Card> card) {
        if (hand.size() < max_cards_in_hand) {
            hand.emplace_back(std::move(card));
        }
    }

    std::unique_ptr<Card> drop_card_from_hand_by_id(std::size_t obj_id);

    const Card *get_card_from_hand_by_id(std::size_t obj_id) const;

    const std::vector<std::unique_ptr<Card>> &get_hand() const {
        return hand;
    }

    bool play_card_by_id(std::size_t card_obj_id, std::size_t target_id);
};

}  // namespace meow::model

#endif