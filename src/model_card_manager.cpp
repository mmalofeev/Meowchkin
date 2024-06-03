#include "model_card_manager.hpp"
#include <cassert>
#include <fstream>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>
#include <vector>
#include "model_card.hpp"
#include "paths_to_binaries.hpp"

namespace meow {

CardManager::CardManager() {
    std::map<std::string, model::CardType> str_to_type{
        {"SPELL", model::CardType::SPELL},
        {"MONSTER", model::CardType::MONSTER},
        {"RACE", model::CardType::RACE},
        {"CLASS", model::CardType::CLASS},
        {"ITEM", model::CardType::ITEM}};

    std::ifstream f(json_info_path);
    ::nlohmann::json data = ::nlohmann::json::parse(f);
    f.close();

    std::size_t counter = 0;
    for (auto &card : data["cards"]) {
        std::string image = card["image"].get<std::string>();
        std::size_t card_id = counter++;
        model::CardType type = str_to_type[card["type"].get<std::string>()];
        bool openable = card["openable"].get<bool>();
        bool storable = card["storable"].get<bool>();
        std::vector<model::Command> verification =
            model::Command::parse(card["verification"].get<std::vector<std::string>>());

        model::CardInfo info(image, card_id, type, openable, storable, verification);

        switch (type) {
            case model::CardType::ITEM:
            case model::CardType::SPELL: {
                std::vector<model::Command> action =
                    model::Command::parse(card["action"].get<std::vector<std::string>>());
                std::vector<model::Command> unwind =
                    model::Command::parse(card["unwind"].get<std::vector<std::string>>());
                int cost = card["cost"].get<int>();

                cards_instances.emplace_back(
                    std::make_unique<model::SpellCardInfo>(std::move(info), cost, action, unwind)
                );
            } break;
            case model::CardType::MONSTER: {
                std::vector<model::Command> buff =
                    model::Command::parse(card["buff"].get<std::vector<std::string>>());
                std::vector<model::Command> stalking_checker =
                    model::Command::parse(card["stalking_checker"].get<std::vector<std::string>>());
                std::vector<model::Command> lewdness =
                    model::Command::parse(card["lewdness"].get<std::vector<std::string>>());
                int power = card["power"].get<int>();
                int treasures = card["treasures"].get<int>();
                bool undead = card["undead"].get<bool>();

                cards_instances.emplace_back(std::make_unique<model::MonsterCardInfo>(
                    std::move(info), buff, stalking_checker, lewdness, power, treasures, undead
                ));
            } break;
            default:
                break;
        }
    }
}

std::unique_ptr<model::Card> CardManager::create_card(std::size_t card_id) const {
    switch (cards_instances.at(card_id)->type) {
        case model::CardType::ITEM:
        case model::CardType::SPELL: {
            auto result = std::make_unique<model::SpellCard>(cards_instances.at(card_id).get());
            obj_id_to_card_id[result->obj_id] = card_id;
            return result;
        } break;
        case model::CardType::MONSTER: {
            auto result = std::make_unique<model::MonsterCard>(cards_instances.at(card_id).get());
            obj_id_to_card_id[result->obj_id] = card_id;
            return result;
        } break;
        default:
            break;
    }
    assert(false);
    return nullptr;
}

void CardManager::delete_obj_id(std::size_t obj_id) {
    assert(obj_id_to_card_id.find(obj_id) != obj_id_to_card_id.end());
    obj_id_to_card_id.erase(obj_id_to_card_id.find(obj_id));
}

const model::CardInfo *CardManager::get_card_info_by_obj_id(std::size_t obj_id) const {
    assert(obj_id_to_card_id.find(obj_id) != obj_id_to_card_id.end());
    return cards_instances[obj_id_to_card_id[obj_id]].get();
}

std::size_t CardManager::get_number_of_cards() const {
    return cards_instances.size();
}

}  // namespace meow
