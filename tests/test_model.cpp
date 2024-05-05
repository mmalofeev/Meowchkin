#include "game_session.hpp"
#include "model_player.hpp"
#include "virtual_machine.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <limits>


TEST(ModelTest, CompileTest) {
    std::vector<std::size_t> users;
    users.push_back(1);
    users.push_back(2);
    meow::model::GameSession session(users);
}

TEST(ModelTest, InitTest) {
    std::vector<std::size_t> users;
    users.push_back(1);
    users.push_back(2);
    meow::model::GameSession session(users);

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
    
    ASSERT_TRUE(players.at(cur).level() == 1);
    new_state = session.current_state->play_card(players[cur].user_id, players[cur].obj_id, players[cur].get_hand().at(0)->obj_id);
    
    ASSERT_TRUE(new_state.get() == session.current_state.get());
    ASSERT_TRUE(players[cur].get_hand().size() == 3);
    ASSERT_TRUE(players[cur ^ 1].get_hand().size() == 4);
    ASSERT_TRUE(players[cur].level() == 2);

    new_state.release();

    new_state = session.current_state->throw_card(players[cur].user_id, players[cur].get_hand().at(0)->obj_id);
    ASSERT_TRUE(new_state.get() == session.current_state.get());
    ASSERT_TRUE(players[cur].get_hand().size() == 2);
    ASSERT_TRUE(players[cur].level() == 2);

    new_state.release();

    new_state = session.current_state->draw_card(players[cur].user_id);
    ASSERT_TRUE(new_state.get() != session.current_state.get());
    ASSERT_TRUE(new_state.get() != nullptr);
    ASSERT_TRUE(players[cur].get_hand().size() == 2);
    ASSERT_TRUE(players[cur].level() == 3);

    session.current_state = std::move(new_state);

    ASSERT_TRUE(session.current_state->type == meow::model::StateType::POSTMANAGEMENT);

    new_state = session.current_state->end_turn(players[cur].user_id);
    session.current_state = std::move(new_state);
    ASSERT_TRUE(session.current_state->type == meow::model::StateType::MANAGEMENT);
    ASSERT_TRUE(players[cur].user_id != session.shared_state.get_current_user_id());

}