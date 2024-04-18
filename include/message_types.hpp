#ifndef MEOWCHKIN_MESSAGE_TYPES_HPP
#define MEOWCHKIN_MESSAGE_TYPES_HPP

#include <nlohmann/json.hpp>

using json = ::nlohmann::json;

namespace meow::network {
class Action {
public:
    bool is_played;
    bool is_drowed;
    bool is_threw;
    int card_id{};
    std::size_t target_player{};
    std::size_t sender_player{};
    // TODO: remove once we will be able to have card by card_id!
    std::string card_filename;

    explicit Action(
        bool is_played,
        bool is_drowed,
        bool is_threw,
        int card_id,
        std::size_t target_player,
        std::size_t sender_player
    )
        : is_played(is_played),
          is_drowed(is_drowed),
          is_threw(is_threw),
          card_id(card_id),
          target_player(target_player),
          sender_player(sender_player) {
    }

    explicit Action(std::string card_filename, std::size_t target_player, std::size_t sender_player)
        : card_filename(std::move(card_filename)),
          target_player(target_player),
          sender_player(sender_player) {
    }

    explicit Action(const json &json) {
        parse_from_json(json);
    }

    void parse_from_json(const json &json) {
        json.at("is_played").get_to(is_played);
        json.at("is_drowed").get_to(is_drowed);
        json.at("is_threw").get_to(is_threw);
        json.at("card_id").get_to(card_id);
        json.at("target_player").get_to(target_player);
        json.at("sender_player").get_to(sender_player);
        json.at("card_filename").get_to(card_filename);
    }

    [[nodiscard]] json to_json() const {
        return json{
            {"type", "Action"},
            {"is_played", is_played},
            {"is_drowed", is_drowed},
            {"is_threw", is_threw},
            {"card_id", card_id},
            {"target_player", target_player},
            {"sender_player", sender_player},
            {"card_filename", card_filename}
        };
    }
};

class ActionResult {
public:
    int card_id{};
    bool validness{};

    explicit ActionResult(int card_id, bool validness) : card_id(card_id), validness(validness) {
    }

    explicit ActionResult(const json &json) {
        parse_from_json(json);
    }

    void parse_from_json(const json &json) {
        json.at("card_id").get_to(card_id);
        json.at("validness").get_to(validness);
    }

    [[nodiscard]] json to_json() const {
        return json{{"type", "ActionResult"}, {"card_id", card_id}, {"validness", validness}};
    }
};

class PlayerInfo {
public:
    std::size_t id{};
    std::string name;

    explicit PlayerInfo(std::size_t id, const std::string &name) : id(id), name(name) {
    }

    explicit PlayerInfo(const json &json) {
        parse_from_json(json);
    }

    void parse_from_json(const json &json) {
        json.at("id").get_to(id);
        json.at("name").get_to(name);
    }

    [[nodiscard]] json to_json() const {
        return json{{"type", "PlayerInfo"}, {"id", id}, {"name", name}};
    }
};

class ChatMessage {
public:
    std::string message;
    std::size_t sender_player{};
    bool general{};
    std::size_t target_player{};

    explicit ChatMessage(std::string message, std::size_t sender_player)
        : message(std::move(message)), sender_player(sender_player), general(true) {
    }

    explicit ChatMessage(std::string message, std::size_t sender_player, std::size_t target_player)
        : message(std::move(message)),
          sender_player(sender_player),
          general(false),
          target_player(target_player) {
    }

    explicit ChatMessage(const json &json) {
        parse_from_json(json);
    }

    void parse_from_json(const json &json) {
        json.at("message").get_to(message);
        json.at("sender_player").get_to(sender_player);
        json.at("general").get_to(general);
        json.at("target_player").get_to(target_player);
    }

    [[nodiscard]] json to_json() const {
        return json{
            {"type", "ChatMessage"},
            {"message", message},
            {"sender_player", sender_player},
            {"general", general},
            {"target_player", target_player}
        };
    }
};
}  // namespace meow::network

#endif  // MEOWCHKIN_MESSAGE_TYPES_HPP
