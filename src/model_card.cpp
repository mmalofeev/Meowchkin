#include "model_card.hpp"
#include "model_card_manager.hpp"
#include "virtual_machine.hpp"

namespace meow::model {

void Card::apply(std::size_t player_id, std::size_t target_id) {
    on_board = true;
    auto target_info = CardManager::get_instance().get_card_info_by_obj_id(target_id);
    bool protogonist_sided = !(info->type == CardType::MONSTER || (target_info != nullptr && target_info->type == CardType::MONSTER));
    for (auto &observer : VirtualMachine::get_instance().get_observers()) {
        observer->on_card_add_on_board(obj_id, protogonist_sided, VirtualMachine::get_instance().get_user_id_by_player_id(player_id));
    }
};

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
    if (on_board) {
        for (auto &observer : VirtualMachine::get_instance().get_observers()) {
            observer->on_card_remove_from_board(obj_id);
        }
    }
}

void SpellCard::apply(std::size_t player_id, std::size_t target_id) {
    VirtualMachine::get_instance().set_args(player_id, target_id);
    VirtualMachine::get_instance().execute(dynamic_cast<const SpellCardInfo *>(info)->action);
    applied = true;
    Card::apply(player_id, target_id);
}

SpellCard::~SpellCard() {
    if (applied) {
        VirtualMachine::get_instance().set_args(player_id, target_id);
        VirtualMachine::get_instance().execute(dynamic_cast<const SpellCardInfo *>(info)->unwind);
    }
}

bool ItemCard::verify(std::size_t player_id, std::size_t target_id) const {
    auto item_info = dynamic_cast<const ItemCardInfo *>(info);
    if (player_id != target_id && !VirtualMachine::get_instance().check_player_item_eligiblity(player_id,  item_info->itype, item_info->bound))
        return false;
    return SpellCard::verify(player_id, target_id);
}

void ItemCard::apply(std::size_t player_id, std::size_t target_id) {
    auto item_info = dynamic_cast<const ItemCardInfo *>(info);
    VirtualMachine::get_instance().acquire_item(player_id, item_info->itype, item_info->bound);
    SpellCard::apply(player_id, target_id);
}

ItemCard::~ItemCard() {
    auto item_info = dynamic_cast<const ItemCardInfo *>(info);
    VirtualMachine::get_instance().acquire_item(player_id, item_info->itype, -item_info->bound);
}

bool MonsterCard::check_stalking(std::size_t target_id) const {
    const auto &checker = dynamic_cast<const MonsterCardInfo *>(info)->stalking_checker;
    if (checker.empty()) {
        return true;
    }
    VirtualMachine::get_instance().set_args(obj_id, target_id);
    return static_cast<bool>(VirtualMachine::get_instance().execute(checker));
}

void MonsterCard::apply_lewdness(std::size_t target_id) {
    VirtualMachine::get_instance().set_args(obj_id, target_id);
    VirtualMachine::get_instance().execute(dynamic_cast<const MonsterCardInfo *>(info)->lewdness);
}

void MonsterCard::apply_buff() {
    if (!is_buffed_flag) {
        VirtualMachine::get_instance().set_args(obj_id);
        VirtualMachine::get_instance().execute(dynamic_cast<const MonsterCardInfo *>(info)->buff);
        is_buffed_flag = true;
    }
}

}  // namespace meow::model