#include "game_session.hpp"
#include "model_player.hpp"
#include "model_object.hpp"
#include "model_card.hpp"
#include "virtual_machine.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <limits>


TEST(ModelTest, CompileTest) {
    std::vector<std::size_t> users;
    users.push_back(1);
    users.push_back(2);
    meow::model::GameSession session{};
    session.init(users);
}

TEST(ModelTest, InitTest) {
    std::vector<std::size_t> users;
    users.push_back(1);
    users.push_back(2);
    meow::model::GameSession session{};
    session.init(users);

    auto &players = session.shared_state.get_all_players();
    auto new_state = session.current_state->roll_dice(players[0].user_id);
    ASSERT_TRUE(new_state.get() == session.current_state.get());

    new_state.release();

    new_state = session.current_state->roll_dice(players[1].user_id);
    ASSERT_TRUE(new_state.get() != session.current_state.get());

    session.current_state = std::move(new_state);

    ASSERT_TRUE(session.current_state->type == meow::model::StateType::MANAGEMENT);
    ASSERT_TRUE(players[0].get_hand().size() == 4);
    ASSERT_TRUE(players[1].get_hand().size() == 4);

    ASSERT_TRUE(session.shared_state.get_current_user_id() != std::numeric_limits<std::size_t>::max());
    int cur = 0;
    if (players[cur].user_id != session.shared_state.get_current_user_id())
        cur = 1;
    
    ASSERT_TRUE(players.at(cur).get_level() == 1);

    bool flag = false;
    for (const auto& card :players[cur].get_hand()) {
        if (card->info->type == meow::model::CardType::SPELL) {
            new_state = session.current_state->play_card(players[cur].user_id, players[cur].obj_id, card->obj_id);
            flag = true;
            break;
        }
    }
    
    if (flag) {
        ASSERT_TRUE(new_state.get() == session.current_state.get());
        ASSERT_TRUE(players[cur].get_hand().size() == 3);
        ASSERT_TRUE(players[cur ^ 1].get_hand().size() == 4);
        ASSERT_TRUE(players[cur].get_level() == 2);

        new_state.release();
    }

    new_state = session.current_state->throw_card(players[cur].user_id, players[cur].get_hand().at(0)->obj_id);
    ASSERT_TRUE(new_state.get() == session.current_state.get());
    ASSERT_TRUE(players[cur].get_hand().size() == 3 - flag);
    ASSERT_TRUE(players[cur].get_level() == 1 + flag);

    new_state.release();
}


TEST(ModelTest, BaseBrowlTest) {
    std::vector<std::size_t> users;
    users.push_back(1);
    users.push_back(2);
    meow::model::GameSession session{};
    session.init(users);

    auto &players = session.shared_state.get_all_players();
    auto new_state = session.current_state->roll_dice(players[0].user_id);
    ASSERT_TRUE(new_state.get() == session.current_state.get());

    new_state.release();

    new_state = session.current_state->roll_dice(players[1].user_id);
    ASSERT_TRUE(new_state.get() != session.current_state.get());

    session.current_state = std::move(new_state);
    int cur = 0;
    if (players[cur].user_id != session.shared_state.get_current_user_id())
        cur = 1;
    

    new_state = session.current_state->draw_card(players[cur].user_id);
    ASSERT_TRUE(new_state.get() != session.current_state.get());
    ASSERT_TRUE(new_state.get() != nullptr);
    ASSERT_TRUE(players[cur].get_hand().size() == 4);
    session.current_state = std::move(new_state);

    if (session.current_state->type != meow::model::StateType::POSTMANAGEMENT) {
        ASSERT_TRUE(session.current_state->type == meow::model::StateType::BRAWL);
        bool flag = dynamic_cast<meow::model::BrawlState*>(session.current_state.get())->are_heroes_leading();
        
        if (!flag) {
            new_state = session.current_state->pass(players[cur].user_id);
            session.current_state = std::move(new_state);
        } else {
            new_state = session.current_state->pass(players[cur ^ 1].user_id);
            session.current_state = std::move(new_state);
        }

        ASSERT_TRUE(session.current_state->type == meow::model::StateType::POSTMANAGEMENT);
        if (flag) {
            ASSERT_TRUE(players[cur].get_hand().size() == 6);
            ASSERT_TRUE(players[cur].get_level() == 3);
        } else {
            ASSERT_TRUE(players[cur].get_hand().size() == 4);
            ASSERT_TRUE(players[cur].get_level() == 1);
        }
    }

    new_state = session.current_state->end_turn(players[cur].user_id);
    session.current_state = std::move(new_state);
    ASSERT_TRUE(session.current_state->type == meow::model::StateType::MANAGEMENT);
    ASSERT_TRUE(players[cur].user_id != session.shared_state.get_current_user_id());
}

TEST(ModelTest, PlaySpellsFromHandBrowlTest) {
    std::vector<std::size_t> users;
    users.push_back(1);
    users.push_back(2);
    meow::model::GameSession session{};
    session.init(users);

    auto &players = session.shared_state.get_all_players();
    auto new_state = session.current_state->roll_dice(players[0].user_id);
    ASSERT_TRUE(new_state.get() == session.current_state.get());

    new_state.release();

    new_state = session.current_state->roll_dice(players[1].user_id);
    ASSERT_TRUE(new_state.get() != session.current_state.get());

    session.current_state = std::move(new_state);
    int cur = 0;
    if (players[cur].user_id != session.shared_state.get_current_user_id())
        cur = 1;

    bool flag = true;
    while(flag) {
        flag = false;
        for (const auto& card :players[cur].get_hand()) {
            if (card->info->type == meow::model::CardType::SPELL) {
                new_state = session.current_state->play_card(players[cur].user_id, players[cur].obj_id, card->obj_id);
                new_state.release();
                flag = true;
                break;
            }
        }
    }
    ASSERT_TRUE(session.current_state != nullptr);
    int level = players[cur].get_level();
    int hand_size = players[cur].get_hand().size();

    new_state = session.current_state->draw_card(players[cur].user_id);
    session.current_state = std::move(new_state);

    if (session.current_state->type != meow::model::StateType::POSTMANAGEMENT) {
        ASSERT_TRUE(session.current_state->type == meow::model::StateType::BRAWL);
        bool flag = dynamic_cast<meow::model::BrawlState*>(session.current_state.get())->are_heroes_leading();
        
        if (!flag) {
            new_state = session.current_state->pass(players[cur].user_id);
            session.current_state = std::move(new_state);
        } else {
            new_state = session.current_state->pass(players[cur ^ 1].user_id);
            session.current_state = std::move(new_state);
        }

        ASSERT_TRUE(session.current_state->type == meow::model::StateType::POSTMANAGEMENT);
        if (flag) {
            ASSERT_TRUE(players[cur].get_hand().size() == hand_size + 1);
            ASSERT_TRUE(players[cur].get_level() == level + 1);
        } else {
            ASSERT_TRUE(players[cur].get_hand().size() == hand_size);
            ASSERT_TRUE(players[cur].get_level() == std::max(level - 2, 1));
        }
    }
}