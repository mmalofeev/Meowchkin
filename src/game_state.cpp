#include "game_state.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include "model_card.hpp"
#include "model_card_manager.hpp"
#include "model_player.hpp"
#include "model_random.hpp"
#include "shared_game_state.hpp"
#include "virtual_machine.hpp"
#include "gui_card.hpp"

namespace meow::model {

EndState::EndState(SharedGameState *shared_state_): GameState(shared_state_, StateType::END) {}

bool BrawlState::is_hero(std::size_t obj_id) const {
    return std::find(heroes.begin(), heroes.end(), obj_id) != heroes.end();
}

bool BrawlState::is_monster(std::size_t obj_id) const {
    return std::find_if(
               monsters.begin(), monsters.end(),
               [&](const std::unique_ptr<MonsterCard> &monster) {
                   return monster->obj_id == obj_id;
               }
           ) != monsters.end();
}

void BrawlState::update_monster(MonsterCard *monster) {
    assert(monster != nullptr);
    assert(is_monster(monster->obj_id));

    for (auto player_id : heroes) {
        if (!monster->is_buffed()) {
            monster->apply_buff();
        }
    }
}

MonsterCard *BrawlState::get_monster(std::size_t obj_id) {
    for (auto &monster : monsters) {
        if (monster->obj_id == obj_id) {
            return monster.get();
        }
    }

    return nullptr;
}

void BrawlState::add_hero(std::size_t player_id) {
    heroes.emplace_back(player_id);
    for (const auto &monster : monsters) {
        update_monster(monster.get());
    }
}

int BrawlState::get_monsters_power() const {
    int total_monster_power = 0;

    for (const auto &monster : monsters) {
        total_monster_power += monster->get_power();
    }

    return total_monster_power;
}

int BrawlState::get_treasures_pool() const {
    int treasures_pool = 0;

    for (const auto &monster : monsters) {
        treasures_pool += monster->get_treasures();
    }

    return treasures_pool;
}

int BrawlState::get_heroes_power() const {
    int total_heroes_power = 0;
    for (auto hero_id : heroes) {
        total_heroes_power += shared_state->get_player_by_player_id(hero_id)->get_power();
    }

    return total_heroes_power;
}

bool BrawlState::are_heroes_leading() const {
    return get_heroes_power() > get_monsters_power();
}

void BrawlState::add_monster(std::unique_ptr<MonsterCard> monster) {
    MonsterCard *monster_ptr = monster.get();
    monsters.emplace_back(std::move(monster));

    update_monster(monster_ptr);
}

BrawlState::BrawlState(
    SharedGameState *shared_state_,
    std::size_t player_id,
    std::unique_ptr<MonsterCard> monster
)
    : GameState(shared_state_, StateType::BRAWL) {
    add_hero(player_id);
    add_monster(std::move(monster));
}

LookForTroubleState::LookForTroubleState(SharedGameState *shared_state_)
    : GameState(shared_state_, StateType::LOOKTROUBLE) {
}

std::unique_ptr<GameState> LookForTroubleState::play_card(std::size_t user_id, std::size_t target_id, std::size_t card_obj_id) {
    if (shared_state->get_current_user_id() != user_id) {
        return nullptr;
    }

    Player *player = shared_state->get_player_by_user_id(user_id);
    auto card = player->get_card_from_hand_by_id(card_obj_id);
    assert(card != nullptr);

    if (card->info->type != CardType::MONSTER || player->obj_id != target_id) {
        return nullptr;
    }

    auto card_instance = player->drop_card_from_hand_by_id(card_obj_id);
    card_instance->apply(player->obj_id, player->obj_id);
    return std::make_unique<BrawlState>(
        shared_state, player->obj_id, dynamic_unique_cast<MonsterCard>(std::move(card_instance))
    );
}

std::unique_ptr<GameState> LookForTroubleState::draw_card(std::size_t user_id) {
    if (shared_state->get_current_user_id() != user_id) {
        return nullptr;
    }

    Player *player = shared_state->get_player_by_user_id(user_id);

    for(size_t i = 0; i < cards_to_deal; i++) {
        player->add_card_to_hand(CardManager::get_instance().create_card(shared_state->get_card_id_from_deck()));
    }

    return std::make_unique<PostManagementState>(shared_state);
}

PostManagementState::PostManagementState(SharedGameState *shared_state_)
    : ManagementState(shared_state_, StateType::POSTMANAGEMENT) {
    assert(shared_state != nullptr);
}

std::unique_ptr<GameState>
BrawlState::play_card(std::size_t user_id, std::size_t target_id, std::size_t card_obj_id) {
    Player *player = shared_state->get_player_by_user_id(user_id);
    auto card = player->get_card_from_hand_by_id(card_obj_id);
    assert(card != nullptr);
    if (card->info->type == CardType::MONSTER) {
        if (card->verify(player->obj_id, target_id)) {
            auto card_instance = std::move(player->drop_card_from_hand_by_id(card_obj_id));
            card_instance->apply(player->obj_id, player->obj_id);
            add_monster(
                dynamic_unique_cast<MonsterCard>(std::move(card_instance))
            );
            return std::unique_ptr<BrawlState>(this);
        }
    } else if (card->info->type == CardType::SPELL) {
        if ((is_hero(target_id) || is_monster(target_id)) &&
            card->verify(player->obj_id, target_id)) {
            auto card_instance = player->drop_card_from_hand_by_id(card_obj_id);
            card_instance->apply(player->obj_id, target_id);
            if (is_hero(target_id)) {
                if (card->info->storable)
                    heroes_storage.emplace_back(std::move(card_instance));
            } else {
                get_monster(target_id)->add_card_to_storage(std::move(card_instance));
            }

            return std::unique_ptr<BrawlState>(this);
        }
    }
    return nullptr;
}

std::unique_ptr<GameState> BrawlState::pass(std::size_t user_id) {
    Player *player = shared_state->get_player_by_user_id(user_id);
    if (passed_players.find(player->obj_id) == passed_players.end()) {
        passed_players.insert(player->obj_id);
        if (is_hero(player->obj_id)) {
            passed_heroes_count++;
        }
    }
    bool heroes_passed = passed_heroes_count == heroes.size();
    bool enemies_passed =
        (passed_players.size() - passed_heroes_count ==
         shared_state->get_number_of_players() - heroes.size());
    int treasures_pool = get_treasures_pool();
    bool heroes_win = are_heroes_leading();
    if (enemies_passed && heroes_win) {
        for (auto hero_id : heroes) {
            for (size_t i = 0; i < treasures_pool; i++) {
                shared_state->get_player_by_player_id(hero_id)->add_card_to_hand(
                    CardManager::get_instance().create_card(shared_state->get_card_id_from_deck())
                );
            }
        }
        shared_state->get_player_by_user_id(shared_state->get_current_user_id())->increase_level(1, true);
        if (shared_state->is_end()) {
            for (auto &observer : VirtualMachine::get_instance().get_observers()) {
                observer->on_game_end(shared_state->get_current_user_id());
            }
            return std::make_unique<EndState>(shared_state);
        }
        return std::make_unique<PostManagementState>(shared_state);
    } else if (heroes_passed && !heroes_win) {
        for (auto hero_id : heroes) {
            for (const auto &monster : monsters) {
                // get_object_based_random_integer<int>(1, 6) < 5
                if (monster->check_stalking(hero_id)) {
                    monster->apply_lewdness(hero_id);
                }
            }
        }
        return std::make_unique<PostManagementState>(shared_state);
    }
    return std::unique_ptr<BrawlState>(this);
}

std::unique_ptr<GameState> PostManagementState::end_turn(std::size_t user_id) {
    if (shared_state->get_current_user_id() != user_id) {
        return nullptr;
    }

    for (auto &observer : VirtualMachine::get_instance().get_observers()) {
        observer->on_turn_end(shared_state->get_current_user_id());
    }

    shared_state->end_turn();

    for (auto &observer : VirtualMachine::get_instance().get_observers()) {
        observer->on_turn_begin(shared_state->get_current_user_id());
    }
    
    return std::make_unique<ManagementState>(shared_state);
}

ManagementState::ManagementState(
    SharedGameState *shared_state_,
    StateType type_ = StateType::MANAGEMENT
)
    : GameState(shared_state_, type_) {
    assert(shared_state != nullptr);
}

std::unique_ptr<GameState>
ManagementState::play_card(std::size_t user_id, std::size_t target_id, std::size_t card_obj_id) {
    if (shared_state->get_current_user_id() != user_id) {
        return nullptr;
    }
    Player *player = shared_state->get_player_by_user_id(user_id);
    auto card = player->get_card_from_hand_by_id(card_obj_id);
    assert(card != nullptr);

    if (card->info->type == CardType::MONSTER) {
        return nullptr;
    }
    if (!player->play_card_by_id(card_obj_id, target_id)) {
        return nullptr;
    }

    return std::unique_ptr<ManagementState>(this);
}

std::unique_ptr<GameState>
ManagementState::throw_card(std::size_t user_id, std::size_t card_obj_id) {
    Player *player = shared_state->get_player_by_user_id(user_id);

    if (player->drop_card_from_hand_by_id(card_obj_id)) {
        return std::unique_ptr<ManagementState>(this);
    } else if (player->drop_card_from_storage_by_id(card_obj_id)) {
        return std::unique_ptr<ManagementState>(this);
    }

    assert(false);
    return nullptr;
}

std::unique_ptr<GameState> ManagementState::draw_card(std::size_t user_id) {
    std::cout << shared_state->get_current_user_id() << " " << user_id << std::endl;
    if (shared_state->get_current_user_id() != user_id) {
        return nullptr;
    }
    Player *player = shared_state->get_player_by_user_id(user_id);

    auto card = CardManager::get_instance().create_card(shared_state->get_card_id_from_deck());

    if (card->info->type != CardType::MONSTER) {
        if (card->info->openable) {
            if (card->verify(player->obj_id, player->obj_id)) {
                card->apply(player->obj_id, player->obj_id);
            } else {
                assert(false);
                return nullptr;
            }
        } else {
            player->add_card_to_hand(std::move(card));
        }
        return std::make_unique<LookForTroubleState>(shared_state);
    }

    card->apply(player->obj_id, player->obj_id);
    return std::make_unique<BrawlState>(
        shared_state, player->obj_id, dynamic_unique_cast<MonsterCard>(std::move(card))
    );
}

InitState::InitState(SharedGameState *shared_state_)
    : GameState(shared_state_, StateType::INIT), results(shared_state->get_number_of_players(), 0) {
    assert(shared_state != nullptr);
    assert(shared_state->get_number_of_players() != 0);
}

std::unique_ptr<GameState> InitState::roll_dice(std::size_t user_id) {
    std::size_t position = shared_state->get_player_position_by_user_id(user_id);
    if (results[position] != 0) {
        return nullptr;
    }

    results[position] = get_object_based_random_integer<int>(1, 6);
    move_count++;
    
    for (auto &observer : VirtualMachine::get_instance().get_observers()) {
        observer->on_dice_roll(user_id, results[position]);
    }

    if (static_cast<std::size_t>(move_count) < results.size()) {
        return std::unique_ptr<InitState>(this);
    }

    std::size_t position_of_first_best_result = 0;
    for (std::size_t i = 1; i < results.size(); i++) {
        if (results[i] > results[position_of_first_best_result]) {
            position_of_first_best_result = i;
        }
    }
    
    shared_state->set_first_player_by_position(position_of_first_best_result);
    for (auto &player : shared_state->get_all_players()) {
        for (size_t i = 0; i < init_hand_size; i++) {
            auto card =
                CardManager::get_instance().create_card(shared_state->get_card_id_from_deck());
            player.add_card_to_hand(std::move(card));
        }
    }
    
    for (auto &observer : VirtualMachine::get_instance().get_observers()) {
        observer->on_turn_begin(shared_state->get_current_user_id());
    }
    return std::make_unique<ManagementState>(shared_state);
}

}  // namespace meow::model
