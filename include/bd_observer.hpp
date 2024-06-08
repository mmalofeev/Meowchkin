#ifndef MEOWCHKIN_BD_OBSERVER_HPP
#define MEOWCHKIN_BD_OBSERVER_HPP
#include <functional>
#include "abstract_observer.hpp"
#include "model_card_manager.hpp"
#include "sqlite/sqlite3.h"

namespace meow {
struct UsageItemInfo {
    int frequency;
    int card_id;

    UsageItemInfo(int freq, int id) : frequency(freq), card_id(id) {
    }
};

bool operator<(const UsageItemInfo &a, const UsageItemInfo &b) {
    return a.frequency > b.frequency;
}

class StatisticObserver : public Observer {
    sqlite3 *DB{nullptr};

    int count_existed_strings() {
        int cnt = 0;
        std::string sql = "SELECT COUNT(*) FROM card_usage";
        auto callback = [](void *data, int argc, char **argv, char **azColName) -> int {
            int *cnt = static_cast<int *>(data);
            *cnt = std::stoi(argv[0]);
            return 0;
        };
        char *errmsg;
        int exit = sqlite3_exec(DB, sql.c_str(), callback, &cnt, &errmsg);
        if (exit) {
            throw std::runtime_error("Failed count vals: " + std::string(errmsg) + "\n");
        }
        return cnt;
    }

    std::vector<int> get_ids_of_all_cards() {
        std::vector<int> ids(CardManager::get_instance().get_number_of_cards());
        for (int i = 0; i < CardManager::get_instance().get_number_of_cards(); ++i) {
            ids[i] = i;
        }
        return ids;
    }

    void init() {
        if (count_existed_strings() == get_ids_of_all_cards().size()) {
            return;
        }
        int was_inited_cnt = count_existed_strings();
        for (int id : get_ids_of_all_cards()) {
            if (id < was_inited_cnt) {
                continue;
            }
            std::string sql =
                "INSERT INTO card_usage (card_id, count_of_usage)\n"
                "VALUES (" +
                std::to_string(id) + ", 0);";
            char *errmsg;
            int exit = sqlite3_exec(DB, sql.c_str(), nullptr, nullptr, &errmsg);
            if (exit) {
                throw std::runtime_error(
                    "Failed init on " + std::to_string(id) + " id: " + std::string(errmsg) + "\n"
                );
            }
        }
    }

    std::size_t get_card_by_obj(std::size_t obj_id) {
        return CardManager::get_instance().get_card_info_by_obj_id(obj_id)->card_id;
    }

public:
    StatisticObserver() {
        std::string sql =
            "CREATE TABLE card_usage("
            "card_id INT,"
            "count_of_usage INT);";
        sqlite3_open("meowchkin.db", &DB);
        sqlite3_exec(DB, sql.c_str(), nullptr, nullptr, nullptr);
        init();
    }

    void display_table() {
        std::string query = "SELECT * FROM card_usage;";
        auto callback = [](void *data, int argc, char **argv, char **azColName) -> int {
            for (int i = 0; i < argc; i++) {
                printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
            }
            printf("\n");
            return 0;
        };
        char *errmsg;
        int exit = sqlite3_exec(DB, query.c_str(), callback, nullptr, &errmsg);
        if (exit) {
            throw std::runtime_error("Failed display: " + std::string(errmsg) + "\n");
        }
    }

    void on_card_add_on_board(std::size_t obj_id, bool protogonist_sided, std::size_t user_id)
        final {
        if (!protogonist_sided) {
            return;
        }
        std::size_t card_id = get_card_by_obj(obj_id);
        std::string sql =
            "UPDATE card_usage SET count_of_usage = count_of_usage + 1 WHERE card_id = " +
            std::to_string(card_id) + ";";
        char *errmsg;
        int exit = sqlite3_exec(DB, sql.c_str(), nullptr, nullptr, &errmsg);
        if (exit) {
            throw std::runtime_error(
                "Failed increase_count_of_usage: " + std::string(errmsg) + "\n"
            );
        }
    }

    std::vector<UsageItemInfo> get_frequency_of_usage_items() {
        std::vector<UsageItemInfo> frequency;
        std::string query = "SELECT * FROM card_usage;";
        auto callback = [](void *data, int argc, char **argv, char **azColName) -> int {
            auto *frequency = static_cast<std::vector<std::pair<int, int>> *>(data);
            (*frequency).emplace_back(std::stoi(argv[1]), std::stoi(argv[0]));
            return 0;
        };
        char *errmsg;
        int exit = sqlite3_exec(DB, query.c_str(), callback, &frequency, &errmsg);
        if (exit) {
            throw std::runtime_error(
                "Failed get_frequency_of_usage: " + std::string(errmsg) + "\n"
            );
        }
        std::sort(frequency.begin(), frequency.end());
        return frequency;
    }

    ~StatisticObserver() {
        sqlite3_close(DB);
    }
};
}  // namespace meow
#endif  // MEOWCHKIN_BD_OBSERVER_HPP
