#ifndef GAME_HPP_
#define GAME_HPP_
#include <vector>
#include "model_object.hpp"
#include "model_player.hpp"

namespace meow::model {

enum class StateType { INIT, PREPARATION, BROWL, LOOKTROUBLE, COMPLETION };

struct Game {
    friend struct VirtualMachine;

private:
    std::vector<Player> players;
    std::size_t cur_turn = 0;
    StateType state = StateType::INIT;
    const Object desk;
    std::vector<std::size_t> deck;

    const std::size_t init_hand_size = 4;

public:
    void start();

    void add_player(std::size_t user_id);

    void add_card_id_to_deck(std::size_t card_id);

    std::size_t get_card_id_from_deck();

    std::size_t get_desk_id() const;

    Player *get_player_by_player_id(std::size_t player_id);

    Player *get_player_by_user_id(std::size_t user_id);

    bool open_door(std::size_t user_id);

    bool end_turn(std::size_t user_id);

    bool is_end() const;
};

}  // namespace meow::model

#endif