#ifndef MEOWCHKIN_ABSTRACT_OBSERVER_HPP
#define MEOWCHKIN_ABSTRACT_OBSERVER_HPP

#include <cstddef>

namespace meow {

class Observer {
public:
    virtual void on_card_add_on_board(std::size_t card_id) = 0;
    virtual void on_card_remove_from_board(std::size_t card_id) = 0;
    virtual void on_turn_begin(std::size_t player_id) = 0;
    virtual void on_turn_end(std::size_t player_id) = 0;
    virtual void on_levelup(std::size_t player_id) = 0;
    virtual void on_card_receive(std::size_t player_id) = 0;
    virtual void on_item_equip(std::size_t player_id) = 0;
    virtual void on_item_loss(std::size_t player_id) = 0;
    virtual void on_monster_elimination(std::size_t player_id) = 0;  // which player killed monster
    virtual void on_being_cursed(std::size_t player_id) = 0;         // which player is cursed
};

}  // namespace meow
#endif  // MEOWCHKIN_ABSTRACT_OBSERVER_HPP
