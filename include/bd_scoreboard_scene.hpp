#ifndef BD_SCORE_BOARD_HPP_
#define BD_SCORE_BOARD_HPP_

#include "model_card_manager.hpp"
#include "scene.hpp"

namespace meow {

class ScoreBoardBase : public Scene {
public:
    CardManager *card_manager = nullptr;
};

}  // namespace meow

#endif  // BD_SCORE_BOARD_HPP_
