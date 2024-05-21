#ifndef GAME_VIEW_HPP_
#define GAME_VIEW_HPP_

#include "abstract_observer.hpp"
#include "model_card_manager.hpp"
#include "scene.hpp"

namespace meow {

class BOOST_SYMBOL_VISIBLE GameView : public Scene, public Observer {
public:
    CardManager *card_manager_ptr = nullptr;
};

}  // namespace meow

#endif  // GAME_VIEW_HPP_
