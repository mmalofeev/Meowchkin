#ifndef MEOWCHKIN_BD_OBSERVER_HPP
#define MEOWCHKIN_BD_OBSERVER_HPP
#include <functional>
#include "sqlite/sqlite3.h"

class StatisticObserver {
    sqlite3 *DB{nullptr};

    int count() {
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

    std::vector<int> get_ids_of_cards() {
        std::vector<int> ids(10);
        for (int i = 0; i < 10; ++i) {
            ids[i] = i;
        }
        return ids;
    }

    void init() {
        if (count() != 0) {
            return;
        }
        for (int id : get_ids_of_cards()) {
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

    void increase_count_of_usage(int card_id) {
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

    std::vector<std::pair<int, int>> get_frequency_of_usage() {
        std::vector<std::pair<int, int>> frequency;  //{frequency, card_id}
        std::string query = "SELECT * FROM card_usage;";
        auto callback = [](void *data, int argc, char **argv, char **azColName) -> int {
            auto *frequency = static_cast<std::vector<std::pair<int, int>> *>(data);
            (*frequency).emplace_back(std::stoi(argv[1]), std::stoi(argv[0]));
            return 0;
        };
        char *errmsg;
        int exit = sqlite3_exec(DB, query.c_str(), callback, &frequency, &errmsg);
        if (exit) {
            throw std::runtime_error("Failed get_frequency_of_usage: " + std::string(errmsg) + "\n");
        }
        std::sort(frequency.begin(), frequency.end());
        return frequency;
    }

    ~StatisticObserver() {
        sqlite3_close(DB);
    }
};

#endif  // MEOWCHKIN_BD_OBSERVER_HPP
