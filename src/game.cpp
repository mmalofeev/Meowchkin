#include "game.hpp"
#include <cassert>
#include "model_card_manager.hpp"
#include "model_player.hpp"

namespace meow::model {

void Game::start() {
    assert(state == StateType::INIT);
    for (auto &player : players) {
        for (std::size_t i = 0; i < init_hand_size; i++) {
            player.add_card_to_hand(
                std::move(CardManager::get_instance().get_card(get_card_id_from_deck()))
            );
        }
    }
    state = StateType::PREPARATION;
}

void Game::add_player(std::size_t user_id) {
    assert(state == StateType::INIT);
    players.emplace_back(user_id);
}

void Game::add_card_id_to_deck(std::size_t card_id) {
    deck.push_back(card_id);
}

std::size_t Game::get_card_id_from_deck() {
    std::size_t res = deck.back();
    deck.pop_back();
    return res;
}

std::size_t Game::get_desk_id() const {
    return desk.obj_id;
}

Player *Game::get_player_by_user_id(std::size_t user_id) {
    for (size_t i = 0; i < players.size(); i++) {
        if (players[i].user_id == user_id) {
            return &players[i];
        }
    }
    assert(false);
}

Player *Game::get_player_by_player_id(std::size_t player_id) {
    for (size_t i = 0; i < players.size(); i++) {
        if (players[i].obj_id == player_id) {
            return &players[i];
        }
    }
    assert(false);
}

bool Game::open_door(std::size_t user_id) {
    if (state != StateType::PREPARATION || user_id != players[cur_turn].user_id) {
        return false;
    }

    assert(!deck.empty());

    auto card = CardManager::get_instance().get_card(deck.back());

    if (card->info->openable) {
        if (card->verify(desk.obj_id, players[cur_turn].obj_id)) {
            card->apply(desk.obj_id, players[cur_turn].obj_id);
        } else if (card->verify(players[cur_turn].obj_id, desk.obj_id)) {
            card->apply(players[cur_turn].obj_id, desk.obj_id);
        } else {
            assert(false);
        }
    } else {
        players[cur_turn].add_card_to_hand(std::move(card));
    }

    state = StateType::COMPLETION;
    return true;
}

bool Game::end_turn(std::size_t user_id) {
    if (state != StateType::COMPLETION || user_id != players[cur_turn].user_id) {
        return false;
    }
    cur_turn = (cur_turn + 1) % players.size();

    state = StateType::PREPARATION;
    return true;
}

bool Game::is_end() const {
    for (std::size_t i = 0; i < players.size(); i++) {
        if (players[i].level() == 10) {
            return true;
        }
    }

    return false;
}

}  // namespace meow::model