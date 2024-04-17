#ifndef CARD_HPP_
#define CARD_HPP_
#include <memory>
#include <string>
#include "model_command.hpp"
#include "model_object.hpp"

namespace meow::model {

enum class CardType { SPELL, MONSTER, RACE, CLASS, ITEM };

struct CardInfo {
    const std::string image;
    const std::size_t card_id;
    const CardType type;
    const bool openable;
    const std::vector<Command> verification;

    CardInfo(
        const std::string &image,
        std::size_t card_id,
        CardType type,
        bool openable,
        const std::vector<Command> &verification
    )
        : image(image),
          card_id(card_id),
          type(type),
          openable(openable),
          verification(verification) {
    }

    virtual ~CardInfo() = default;
};

struct SpellCardInfo : CardInfo {
public:
    const std::vector<Command> action;
    const std::vector<Command> unwind; // will be called when the action needs to be canceled
    const bool storable;
    const bool is_one_time;

    SpellCardInfo(
        CardInfo base,
        bool storable,
        bool is_one_time,
        const std::vector<Command> &action,
        const std::vector<Command> &unwind
    )
        : CardInfo(std::move(base)),
          action(action),
          unwind(unwind),
          storable(storable),
          is_one_time(is_one_time) {
    }
};

struct MonsterCardInfo : CardInfo {
public:
    const std::vector<Command> buff;
    const std::vector<Command> lewdness;

    MonsterCardInfo(
        CardInfo base,
        const std::vector<Command> &buff,
        const std::vector<Command> &lewdness
    )
        : CardInfo(std::move(base)), buff(buff), lewdness(lewdness) {
    }
};

struct Card : Object {
public:
    const CardInfo *info;

    Card(CardInfo *info) : info(info) {
    }

    bool verify(std::size_t player_id, std::size_t target_id) const;
    virtual void apply(std::size_t player_id, std::size_t target_id) = 0;
};

struct SpellCard : Card {
private:
    bool applied = false;
    std::size_t player_id{};
    std::size_t target_id{};

public:
    SpellCard(CardInfo *info) : Card(info) {
    }

    void apply(std::size_t player_id, std::size_t target_id) override;
    ~SpellCard();
};

/*
struct MonsterCard: Card {
private:
    int power{};
    bool is_pursuer{};
public:

};
*/

}  // namespace meow::model

#endif