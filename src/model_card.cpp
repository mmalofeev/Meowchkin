#include "model_card.hpp"
#include "model_card_manager.hpp"
#include "virtual_machine.hpp"

namespace meow::model {

bool Card::verify(std::size_t player_id, std::size_t target_id) const {
    assert(info != nullptr);
    if (info->verification.empty()) {
        return true;
    }
    VirtualMachine::get_instance().set_args(player_id, target_id);
    return static_cast<bool>(*VirtualMachine::get_instance().execute(info->verification));
}

Card::~Card() {
    CardManager::get_instance().delete_obj_id(obj_id);
}

void SpellCard::apply(std::size_t player_id, std::size_t target_id) {
    VirtualMachine::get_instance().set_args(player_id, target_id);
    VirtualMachine::get_instance().execute(dynamic_cast<const SpellCardInfo *>(info)->action);
    applied = true;
}

SpellCard::~SpellCard() {
    if (applied) {
        VirtualMachine::get_instance().execute(dynamic_cast<const SpellCardInfo *>(info)->unwind);
    }
}

}  // namespace meow::model