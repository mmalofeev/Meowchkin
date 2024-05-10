#ifndef CARD_MANAGER_HPP_
#define CARD_MANAGER_HPP_
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "model_card.hpp"

namespace meow {

template <typename To, typename From>
std::unique_ptr<To> dynamic_unique_cast(std::unique_ptr<From> &&p) {
    if (To *cast = dynamic_cast<To *>(p.get())) {
        std::unique_ptr<To> result(cast);
        p.release();
        return result;
    }
    return std::unique_ptr<To>(nullptr);
}

struct CardManager {
private:
    std::vector<std::unique_ptr<model::CardInfo>> cards_instances;
    mutable std::map<std::size_t, std::size_t> obj_id_to_card_id;
    CardManager();

public:
    static CardManager &get_instance() {
        thread_local static CardManager instance;
        return instance;
    }

    std::unique_ptr<model::Card> get_card(std::size_t card_id) const;
    const model::CardInfo *get_card_info_by_obj_id(std::size_t obj_id) const;
    std::size_t get_number_of_cards() const;
    void delete_obj_id(std::size_t obj_id);
};

}  // namespace meow

#endif