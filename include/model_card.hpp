#ifndef CARD_HPP_
#define CARD_HPP_
#include <memory>
#include <string>
#include "model_command.hpp"
#include "model_object.hpp"

namespace meow::model {

enum class CardType { SPELL, MONSTER, RACE, CLASS, ITEM };
enum class ItemType { BOOTS, HELMET, BREASTPLATE, WEAPON };

struct CardInfo {
    const std::string image;
    const std::size_t card_id;
    const CardType type;
    const bool openable;
    const bool storable;
    const std::vector<Command> verification;

    CardInfo(
        const std::string &image,
        std::size_t card_id,
        CardType type,
        bool openable,
        bool storable,
        const std::vector<Command> &verification
    )
        : image(image),
          card_id(card_id),
          type(type),
          openable(openable),
          storable(storable),
          verification(verification) {
    }

    virtual ~CardInfo() = default;
};

struct SpellCardInfo : CardInfo {
public:
    const std::vector<Command> action;
    const std::vector<Command> unwind;  // will be called when the action needs to be canceled
    const int cost;

    SpellCardInfo(
        CardInfo base,
        int cost,
        const std::vector<Command> &action,
        const std::vector<Command> &unwind
    )
        : CardInfo(std::move(base)), action(action), unwind(unwind), cost(cost) {
    }
};

struct ItemCardInfo : SpellCardInfo {
public:
    const ItemType itype;
    const int bound;

    ItemCardInfo(
        CardInfo base,
        int cost,
        const std::vector<Command> &action,
        const std::vector<Command> &unwind,
        ItemType itype,
        int bound
    )
        : SpellCardInfo(std::move(base), cost, action, unwind), itype(itype), bound(bound) {
    }
};

struct MonsterCardInfo : CardInfo {
public:
    const std::vector<Command> buff_checker;
    const std::vector<Command> buff;
    const std::vector<Command> stalking_checker;
    const std::vector<Command> lewdness;
    const int power;
    const int treasures;
    const bool undead;

    MonsterCardInfo(
        CardInfo base,
        const std::vector<Command> &buff,
        const std::vector<Command> &stalking_checker,
        const std::vector<Command> &lewdness,
        const int power,
        const int treasures,
        const bool undead
    )
        : CardInfo(std::move(base)),
          buff(buff),
          stalking_checker(stalking_checker),
          lewdness(lewdness),
          power(power),
          treasures(treasures),
          undead(undead) {
    }
};

struct Card : Object {
protected:
    bool on_board = false;

public:
    const CardInfo *info;

    Card(CardInfo *info) : info(info) {
    }

    virtual bool verify(std::size_t player_id, std::size_t target_id) const;
    //use when card is played on board
    virtual void apply(std::size_t player_id, std::size_t target_id);

    virtual ~Card();
};

struct SpellCard : Card {
protected:
    bool applied = false;
    std::size_t player_id{};
    std::size_t target_id{};

public:
    SpellCard(CardInfo *info) : Card(info) {
    }

    void apply(std::size_t player_id, std::size_t target_id) override;
    ~SpellCard();
};

struct ItemCard : SpellCard {
public:
    ItemCard(CardInfo *info) : SpellCard(info) {
    }

    bool verify(std::size_t player_id, std::size_t target_id) const override;
    void apply(std::size_t player_id, std::size_t target_id) override;
    ~ItemCard();
};

struct MonsterCard : Card {
private:
    int power_buff = 0;
    int treasures_buff = 0;
    bool is_buffed_flag = false;
    std::vector<std::unique_ptr<Card>> storage;

public:
    MonsterCard(CardInfo *info) : Card(info) {
    }

    bool check_stalking(std::size_t target_id) const;
    void apply_lewdness(std::size_t target_id);
    void apply_buff();

    void add_card_to_storage(std::unique_ptr<Card> card) {
        storage.emplace_back(std::move(card));
    }

    int get_power() const {
        return dynamic_cast<const MonsterCardInfo *>(info)->power + power_buff;
    }

    int get_treasures() const {
        return dynamic_cast<const MonsterCardInfo *>(info)->treasures + treasures_buff;
    }

    bool is_buffed() const {
        return is_buffed_flag;
    }
};

}  // namespace meow::model

#endif