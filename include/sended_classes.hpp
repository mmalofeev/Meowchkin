#ifndef MEOWCHKIN_SENDED_CLASSES_HPP
#define MEOWCHKIN_SENDED_CLASSES_HPP

#include <nlohmann/json.hpp>

using json = ::nlohmann::json;

namespace meow {
class Action {
 public:
  int card_id{};
  std::size_t targeted_player{};
  std::size_t sender_player{};

  explicit Action(int card_id, std::size_t targeted_player,
                  std::size_t sender_player)
      : card_id(card_id),
        targeted_player(targeted_player),
        sender_player(sender_player) {}

  explicit Action(const json& json) { parse_from_json(json); }

  void parse_from_json(const json& json) {
    json.at("card_id").get_to(card_id);
    json.at("targeted_player").get_to(targeted_player);
    json.at("sender_player").get_to(sender_player);
  }

  [[nodiscard]] json to_json() const {
    return json{{"card_id", card_id},
                {"targeted_player", targeted_player},
                {"sender_player", sender_player}};
  }
};

class Feedback {
 public:
  int card_id{};
  bool validness{};

  explicit Feedback(int card_id, bool validness)
      : card_id(card_id), validness(validness) {}

  explicit Feedback(const json& json) { parse_from_json(json); }

  void parse_from_json(const json& json) {
    json.at("card_id").get_to(card_id);
    json.at("validness").get_to(validness);
  }

  [[nodiscard]] json to_json() const {
    return json{{"card_id", card_id}, {"validness", validness}};
  }
};

class PlayerInfo {
 public:
  std::size_t id{};
  std::string name;

  explicit PlayerInfo(std::size_t id, const std::string& name)
      : id(id), name(name) {}

  explicit PlayerInfo(const json& json) { parse_from_json(json); }

  void parse_from_json(const json& json) {
    json.at("id").get_to(id);
    json.at("name").get_to(name);
  }

  [[nodiscard]] json to_json() const {
    return json{{"id", id}, {"name", name}};
  }
};
}  // namespace meow

#endif  // MEOWCHKIN_SENDED_CLASSES_HPP
