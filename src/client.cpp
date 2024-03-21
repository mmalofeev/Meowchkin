#include "client.hpp"
#include <boost/asio.hpp>
#include <iostream>
#include <nlohmann/json.hpp>

using boost::asio::ip::tcp;

namespace meow::network {
Client::Client() : io_context() {
}

Client::~Client() = default;

Client &Client::get_instance() {
    static Client instance;
    return instance;
}

void Client::accept_info_about_players() {
    std::size_t count_of_players;
    connection >> count_of_players;
    connection >> id_of_client;

    for (std::size_t i = 0; i < count_of_players; ++i) {
        std::string input_msg;
        while (input_msg.empty()) {
            std::getline(connection, input_msg);
        }
        auto json = json::parse(input_msg);
        players_info.emplace_back(json);
    }
}

void Client::connect(const std::string &host) {
    size_t delimiter_pos = host.find(':');
    std::string ip = host.substr(0, delimiter_pos);
    std::string port = host.substr(delimiter_pos + 1);

    tcp::socket s(io_context);
    boost::asio::connect(s, tcp::resolver(io_context).resolve(ip, port));
    connection = tcp::iostream(std::move(s));

    connection << name_of_client + "\n" << std::flush;
    accept_info_about_players();

    std::thread([this]() mutable {
        while (connection) {
            std::string received_msg;
            if (!std::getline(connection, received_msg)) {
                break;
            }
            if (received_msg.empty()) {
                continue;
            }
            auto json = json::parse(received_msg);
            std::unique_lock l(mtx);
            std::string received_type;
            json.at("type").get_to(received_type);
            if (received_type == "Action") {
                received_actions.emplace(json);
            }
            if (received_type == "ActionResult") {
                received_action_results.emplace(json);
            }
            if (received_type == "ChatMessage") {
                received_chat_messages.emplace(json);
            }
        }
    }).detach();
}

void Client::disconnect() {
    connection.close();
}

[[nodiscard]] std::size_t Client::get_id_of_client() const {
    return id_of_client;
}

[[nodiscard]] std::string Client::get_name_of_client() const {
    return name_of_client;
}

void Client::set_name_of_client(const std::string &name) {
    name_of_client = name;
}

[[nodiscard]] const std::vector<PlayerInfo> &Client::get_players_info() const {
    return players_info;
}

void Client::send_action(const Action &action) {
    auto json = action.to_json();
    std::string message_to_send;
    message_to_send = json.dump();
    message_to_send += "\n";
    connection << message_to_send << std::flush;
}

std::optional<Action> Client::receive_action() {
    std::unique_lock l(mtx);
    if (received_actions.empty()) {
        return std::nullopt;
    }
    Action action = received_actions.front();
    received_actions.pop();
    return action;
}

std::optional<ActionResult> Client::receive_action_result() {
    std::unique_lock l(mtx);
    if (received_action_results.empty()) {
        return std::nullopt;
    }
    ActionResult action_result = received_action_results.front();
    received_action_results.pop();
    return action_result;
}

void Client::send_chat_message(const ChatMessage &chat_message) {
    auto json = chat_message.to_json();
    std::string message_to_send;
    message_to_send = json.dump();
    message_to_send += "\n";
    connection << message_to_send << std::flush;
}

std::optional<ChatMessage> Client::receive_chat_message() {
    std::unique_lock l(mtx);
    if (received_chat_messages.empty()) {
        return std::nullopt;
    }
    ChatMessage chat_message = received_chat_messages.front();
    received_chat_messages.pop();
    return chat_message;
}
}  // namespace meow::network