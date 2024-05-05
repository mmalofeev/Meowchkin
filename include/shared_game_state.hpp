#ifndef SHARED_GAME_STATE_HPP_
#define SHARED_GAME_STATE_HPP_
#include <limits>
#include <vector>
#include "game_state.hpp"
#include "model_player.hpp"

namespace meow::model {

struct SharedGameState {
private:
    std::vector<Player> players;
    std::size_t cur_turn = std::numeric_limits<std::size_t>::max();

public:
    SharedGameState(const std::vector<std::size_t> &users);

    std::size_t get_current_user_id() const;

    std::size_t get_number_of_players() const;

    std::size_t get_card_id_from_deck();

    Player *get_player_by_player_id(std::size_t player_id);

    Player *get_player_by_user_id(std::size_t user_id);

    std::size_t get_player_position_by_user_id(std::size_t user_id) const;

    void set_first_player_by_position(std::size_t position);

    std::vector<Player> &get_all_players();

    void end_turn();

    bool is_end() const;
};

}  // namespace meow::model

#endif