#include "model_player.hpp"
#include "virtual_machine.hpp"

namespace meow::model {

bool Player::play_card_by_id(std::size_t card_obj_id, std::size_t target_id) {
    VirtualMachine::get_instance().set_args(obj_id, target_id);
    if (!(*VirtualMachine::get_instance().execute(
            get_card_from_hand_by_id(card_obj_id)->info->verification
        ))) {
        return false;
    }

    auto card = drop_card_from_hand_by_id(card_obj_id);
    card->apply(obj_id, target_id);
    if (card->info->type == CardType::SPELL) {
        if (dynamic_cast<const SpellCardInfo *>(card->info)->storable) {
            add_card_to_storage(std::move(card));
        }
    }

    return true;
}

std::unique_ptr<Card> Player::drop_card_from_hand_by_id(std::size_t obj_id) {
    std::unique_ptr<Card> card;

    for (auto it = hand.begin(); it != hand.end(); ++it) {
        if ((*it)->obj_id == obj_id) {
            card = std::move(*it);
            hand.erase(it);
            break;
        }
    }

    return card;
}

const Card *Player::get_card_from_hand_by_id(std::size_t obj_id) const {
    for (size_t i = 0; i < hand.size(); i++) {
        if (hand[i]->obj_id == obj_id) {
            return hand[i].get();
        }
    }
    assert(false);
    return nullptr;
}

}  // namespace meow::model