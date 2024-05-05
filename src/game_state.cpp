#include "game_state.hpp"
#include <cassert>
#include <vector>
#include "model_card_manager.hpp"
#include "model_player.hpp"
#include "model_random.hpp"
#include "shared_game_state.hpp"

namespace meow::model {

PostManagementState::PostManagementState(SharedGameState *shared_state_)
    : ManagementState(shared_state_, StateType::POSTMANAGEMENT) {
    assert(shared_state != nullptr);
}

std::unique_ptr<GameState> PostManagementState::end_turn(std::size_t user_id) {
    if (shared_state->get_current_user_id() != user_id) {
        return nullptr;
    }
    shared_state->end_turn();
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

    if ([[maybe_unused]] auto card = player->drop_card_from_hand_by_id(card_obj_id)) {
        return std::unique_ptr<ManagementState>(this);
    } else if ([[maybe_unused]] auto card = player->drop_card_from_storage_by_id(card_obj_id)) {
        return std::unique_ptr<ManagementState>(this);
    }

    assert(false);
    return nullptr;
}

std::unique_ptr<GameState> ManagementState::draw_card(std::size_t user_id) {
    if (shared_state->get_current_user_id() != user_id) {
        return nullptr;
    }
    Player *player = shared_state->get_player_by_user_id(user_id);

    auto card = CardManager::get_instance().get_card(shared_state->get_card_id_from_deck());

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
        return std::make_unique<PostManagementState>(shared_state);
    }
    return nullptr;
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
    count++;

    if (static_cast<std::size_t>(count) < results.size()) {
        return std::unique_ptr<InitState>(this);
    }

    std::size_t position_of_first_best_result = results[0];
    for (std::size_t i = 1; i < results.size(); i++) {
        if (results[i] > results[position_of_first_best_result]) {
            position_of_first_best_result = i;
        }
    }

    shared_state->set_first_player_by_position(position_of_first_best_result);
    for (auto &player : shared_state->get_all_players()) {
        for (size_t i = 0; i < init_hand_size; i++) {
            player.add_card_to_hand(
                CardManager::get_instance().get_card(shared_state->get_card_id_from_deck())
            );
        }
    }

    return std::make_unique<ManagementState>(shared_state);
}

}  // namespace meow::model