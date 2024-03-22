#ifndef COMMAND_HPP_
#define COMMAND_HPP_
#include <nlohmann/json.hpp>
#include <vector>

namespace meow::model {

enum class CommandType {
    STACK_LOAD,
    STACK_POP,
    STACK_DOUBL,
    INCREASE_LEVEL,
    INCREASE_LEVEL_FORCE,
    EQ,
    DENY,
    IF,
    IS_DESK,
    IS_USER,
    RETURN,
    LABEL,
    GOTO
};

struct Command {
public:
    const CommandType type;
    const int value;

    static std::vector<Command> parse(const std::vector<std::string> &arr) {
        static std::map<std::string, CommandType> code_to_command{
            {"STACK_LOAD", CommandType::STACK_LOAD},
            {"STACK_POP", CommandType::STACK_POP},
            {"STACK_DOUBL", CommandType::STACK_DOUBL},
            {"INCREASE_LEVEL", CommandType::INCREASE_LEVEL},
            {"INCREASE_LEVEL_FORCE", CommandType::INCREASE_LEVEL_FORCE},
            {"EQ", CommandType::EQ},
            {"DENY", CommandType::DENY},
            {"IF", CommandType::IF},
            {"IS_DESK", CommandType::IS_DESK},
            {"IS_USER", CommandType::IS_USER},
            {"RETURN", CommandType::RETURN},
            {"GOTO", CommandType::GOTO},
            {"LABEL", CommandType::LABEL}};

        std::vector<Command> res;

        for (std::size_t i = 0; i < arr.size(); i++) {
            CommandType command = code_to_command[arr[i]];
            if (command == CommandType::STACK_LOAD) {
                res.push_back({command, std::stoi(arr.at(++i))});
            } else {
                res.push_back({command, 0});
            }
        }
        return res;
    }
};

}  // namespace meow::model

#endif
