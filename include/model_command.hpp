#ifndef COMMAND_HPP_
#define COMMAND_HPP_
#include <nlohmann/json.hpp>
#include <vector>

namespace meow::model {

enum class CommandType {
    STACK_LOAD,
    STACK_POP,
    STACK_DOUBLE,
    LOSE_RANDOM_CARD_FROM_HAND,
    INCREASE_POWER,
    INCREASE_LEVEL,
    INCREASE_LEVEL_FORCE,
    EQ,
    DENY,
    IF,
    IS_DESK,
    IS_USER,
    IS_HAND_EMPTY,
    RETURN,
    LABEL,
    GOTO
};

struct Command {
public:
    const CommandType type;
    const int value;

    static std::vector<Command> parse(const std::vector<std::string> &arr) {
        thread_local static std::map<std::string, CommandType> code_to_command{
            {"STACK_LOAD", CommandType::STACK_LOAD},
            {"STACK_POP", CommandType::STACK_POP},
            {"STACK_DOUBLE", CommandType::STACK_DOUBLE},
            {"INCREASE_POWER", CommandType::INCREASE_POWER},
            {"INCREASE_LEVEL", CommandType::INCREASE_LEVEL},
            {"INCREASE_LEVEL_FORCE", CommandType::INCREASE_LEVEL_FORCE},
            {"LOSE_RANDOM_CARD_FROM_HAND", CommandType::LOSE_RANDOM_CARD_FROM_HAND},
            {"EQ", CommandType::EQ},
            {"DENY", CommandType::DENY},
            {"IF", CommandType::IF},
            {"IS_DESK", CommandType::IS_DESK},
            {"IS_USER", CommandType::IS_USER},
            {"IS_HAND_EMPTY", CommandType::IS_HAND_EMPTY},
            {"RETURN", CommandType::RETURN},
            {"GOTO", CommandType::GOTO},
            {"LABEL", CommandType::LABEL}};

        std::vector<Command> res;

        std::map<std::string, int> label_to_line;

        int line = 0;
        for (std::size_t i = 0; i < arr.size(); i++, line++) {
            CommandType command = code_to_command[arr[i]];
            if (command == CommandType::STACK_LOAD || command == CommandType::GOTO) {
                i++;
            } else if (command == CommandType::LABEL) {
                label_to_line[arr.at(++i)] = line;
            }
        }

        for (std::size_t i = 0; i < arr.size(); i++) {
            CommandType command = code_to_command[arr[i]];
            if (command == CommandType::STACK_LOAD) {
                res.push_back({command, std::stoi(arr.at(++i))});
            } else if (command == CommandType::GOTO) {
                res.push_back({command, label_to_line[arr.at(++i)]});
            } else if (command == CommandType::LABEL) {
                res.push_back({command, 0});
                i++;
            } else {
                res.push_back({command, 0});
            }
        }
        return res;
    }
};

}  // namespace meow::model

#endif
