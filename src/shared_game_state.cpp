#include "shared_game_state.hpp"
#include <cassert>
#include <limits>
#include "model_card_manager.hpp"
#include "model_player.hpp"
#include "model_random.hpp"

namespace meow::model {

void SharedGameState::set_player_list(const std::vector<std::size_t> &users) {
    for (std::size_t i = 0; i < users.size(); i++) {
        players.emplace_back(users[i]);
    }
}

std::size_t SharedGameState::get_player_position_by_user_id(std::size_t user_id) const {
    for (size_t i = 0; i < players.size(); i++) {
        if (players[i].user_id == user_id) {
            return i;
        }
    }

    assert(false);
    return std::numeric_limits<std::size_t>::max();
}

void SharedGameState::set_first_player_by_position(std::size_t position) {
    assert(position < players.size());
    cur_turn = position;
}

std::size_t SharedGameState::get_current_user_id() const {
    assert(cur_turn < players.size());
    return players[cur_turn].user_id;
}

std::size_t SharedGameState::get_number_of_players() const {
    return players.size();
}

std::size_t SharedGameState::get_card_id_from_deck() {
    return get_object_based_random_integer<std::size_t>(
        0, CardManager::get_instance().get_number_of_cards() - 1
    );
}

Player *SharedGameState::get_player_by_user_id(std::size_t user_id) {
    for (size_t i = 0; i < players.size(); i++) {
        if (players[i].user_id == user_id) {
            return &players[i];
        }
    }
    assert(false);
    return nullptr;
}

Player *SharedGameState::get_player_by_player_id(std::size_t player_id) {
    for (size_t i = 0; i < players.size(); i++) {
        if (players[i].obj_id == player_id) {
            return &players[i];
        }
    }
    assert(false);
    return nullptr;
}

std::vector<Player> &SharedGameState::get_all_players() {
    return players;
}

void SharedGameState::end_turn() {
    cur_turn = (cur_turn + 1) % (players.size());
}

bool SharedGameState::is_end() const {
    for (std::size_t i = 0; i < players.size(); i++) {
        if (players[i].get_level() == 10) {
            return true;
        }
    }

    return false;
}

}  // namespace meow::model