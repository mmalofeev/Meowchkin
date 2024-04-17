#ifndef CARD_MANAGER_HPP_
#define CARD_MANAGER_HPP_
#include <memory>
#include <string>
#include <vector>
#include "model_card.hpp"

namespace meow {

struct CardManager {
private:
    std::vector<std::unique_ptr<model::CardInfo>> cards_instances;
    CardManager();

public:
    static CardManager &get_instance() {
        thread_local static CardManager instance;
        return instance;
    }

    std::unique_ptr<model::Card> get_card(std::size_t card_id) const;
};

}  // namespace meow

#endif