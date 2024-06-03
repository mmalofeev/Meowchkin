#ifndef MEOWCHKIN_ABSTRACT_OBSERVER_HPP
#define MEOWCHKIN_ABSTRACT_OBSERVER_HPP

#include <cstddef>

namespace meow {

class Observer {
public:
    virtual void on_card_add_on_board(
        std::size_t card_id,
        bool protogonist_sided,  // bonus to protogonist or opponent
        int bonus,
        std::size_t user_id_applyied  // id of player who gets bonus
    ) {
    }
  
    virtual void on_card_remove_from_board(std::size_t card_id) {
    }
  
    virtual void on_turn_begin(std::size_t user_id) {
    }
  
    virtual void on_turn_end(std::size_t user_id) {
    }
  
    virtual void on_level_change(std::size_t user_id, int delta) {
    }
  
    virtual void on_card_receive(std::size_t user_id, std::size_t card_id) {
    }
  
    virtual void on_item_loss(std::size_t user_id, std::size_t card_id) {
    }
  
    virtual void on_monster_elimination(std::size_t user_id) {  // which player killed monster
    }
    virtual void on_being_cursed(std::size_t user_id) {  // which player is cursed
    }
  
    virtual void on_dice_roll() {
    }
  
    virtual ~Observer() = default;
};

}  // namespace meow
#endif  // MEOWCHKIN_ABSTRACT_OBSERVER_HPP
