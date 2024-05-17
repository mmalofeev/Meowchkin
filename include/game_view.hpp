#ifndef GAME_VIEW_HPP_
#define GAME_VIEW_HPP_

#include <string_view>
#include "model_card_manager.hpp"
#include "scene.hpp"

namespace meow {

class BOOST_SYMBOL_VISIBLE GameView : public Scene {
public:
    // TODO: string -> size_t
    virtual void on_card_add(std::size_t card_id) = 0;
    virtual void on_card_remove(std::string_view card_filename) = 0;
    virtual void on_turn_begin() = 0;
    virtual void on_turn_end() = 0;
    virtual void on_levelup() = 0;
    virtual void on_card_receive() = 0;
    virtual void on_item_equip() = 0;
    virtual void on_item_loss() = 0;
    virtual void on_monster_elimination() = 0;
    virtual void on_being_cursed() = 0;

    CardManager *card_manager_ptr = nullptr;
};

}  // namespace meow

#endif  // GAME_VIEW_HPP_
