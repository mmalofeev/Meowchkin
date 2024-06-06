#ifndef GAME_VIEW_HPP_
#define GAME_VIEW_HPP_

#include "abstract_observer.hpp"
#include "game_session.hpp"
#include "model_card_manager.hpp"
#include "scene.hpp"

namespace meow {

class BOOST_SYMBOL_VISIBLE GameView : public Scene, public Observer {
public:
    CardManager *card_manager = nullptr;
    model::GameSession *game_session = nullptr;

    virtual ~GameView() = default;
};

}  // namespace meow

#endif  // GAME_VIEW_HPP_
