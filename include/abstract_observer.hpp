#ifndef MEOWCHKIN_ABSTRACT_OBSERVER_HPP
#define MEOWCHKIN_ABSTRACT_OBSERVER_HPP

#include <cstddef>

namespace meow {

class Observer {
public:
    virtual void on_card_add(std::size_t obj_id){};     // add to table
    virtual void on_card_remove(std::size_t obj_id){};  // remove from table
    virtual void on_turn_begin(){};
    virtual void on_turn_end(){};
    virtual void on_levelup(){};
    virtual void on_card_receive(){};  // add to player hand
    virtual void on_item_equip(std::size_t obj_id){};
    virtual void on_item_loss(){};
    virtual void on_monster_elimination(){};
    virtual void on_being_cursed(){};
};

}  // namespace meow
#endif  // MEOWCHKIN_ABSTRACT_OBSERVER_HPP
