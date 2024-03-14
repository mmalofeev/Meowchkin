#include "server.hpp"
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include <optional>

using boost::asio::ip::tcp;

namespace meow::network {
Server::Server() : io_context(), acceptor(io_context, tcp::endpoint(tcp::v4(), 0)) {
}

Server::~Server() = default;

Server &Server::get_instance() {
    static Server instance;
    return instance;
}

[[nodiscard]] std::string Server::get_port() const {
    return std::to_string(acceptor.local_endpoint().port());
}

[[nodiscard]] std::vector<std::size_t> Server::get_clients_id() const {
    std::vector<std::size_t> clients_id;
    clients_id.reserve(client_threads.size());
    for (auto &info : players_info) {
        clients_id.push_back(info.id);
    }
    return clients_id;
}

void Server::handle_client(tcp::socket &socket) {
    tcp::iostream client(std::move(socket));
    std::size_t client_id = std::hash<std::thread::id>{}(std::this_thread::get_id());
    std::string client_name;
    client >> client_name;

    mtx.lock();
    players_info.emplace_back(client_id, client_name);
    client_streams[client_id] = &client;
    mtx.unlock();
    got_players_info.notify_one();

    while (client) {
        std::string received_msg;
        if (!std::getline(client, received_msg)) {
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
        if (received_type == "ChatMessage") {
            ChatMessage chat_message(json);
            send_chat_message_to_all_clients(chat_message);
        }
    }
}

void Server::send_players_info(std::size_t client_id) {
    std::vector<std::string> messages;
    for (auto &info : players_info) {
        auto json = info.to_json();
        std::string msg = json.dump();
        msg += "\n";
        messages.push_back(msg);
    }
    auto *stream = client_streams[client_id];
    *stream << messages.size() << '\n' << std::flush;
    *stream << client_id << '\n' << std::flush;
    for (std::string &msg : messages) {
        *stream << msg << std::flush;
    }
}

void Server::start_listening(std::size_t num_of_clients) {
    std::size_t count_of_accepted_clients = 0;
    while (count_of_accepted_clients < num_of_clients) {
        tcp::socket socket = acceptor.accept();
        std::thread new_thread =
            std::thread([s = std::move(socket), this]() mutable { handle_client(s); });
        new_thread.detach();
        client_threads.push_back(std::move(new_thread));
        ++count_of_accepted_clients;
    }
    std::unique_lock l(mtx);
    got_players_info.wait(l, [this, &num_of_clients]() {
        return players_info.size() == num_of_clients;
    });
    for (std::size_t id : get_clients_id()) {
        send_players_info(id);
    }
}

void Server::send_action(std::size_t client_id, const Action &action) {
    auto json = action.to_json();
    std::string message_to_send;
    message_to_send = json.dump();
    message_to_send += "\n";
    auto *stream = client_streams[client_id];
    *stream << message_to_send << std::flush;
}

std::optional<Action> Server::receive_action() {
    std::unique_lock l(mtx);
    if (received_actions.empty()) {
        return std::nullopt;
    }
    Action action = received_actions.front();
    received_actions.pop();
    return action;
}

void Server::send_action_result(std::size_t client_id, const ActionResult &action_result) {
    auto json = action_result.to_json();
    std::string message_to_send;
    message_to_send = json.dump();
    message_to_send += '\n';
    auto stream = client_streams[client_id];
    *stream << message_to_send << std::flush;
}

void Server::send_action_to_all_clients(const Action &action) {
    for (std::size_t id : get_clients_id()) {
        send_action(id, action);
    }
}

void Server::send_chat_message(std::size_t client_id, const ChatMessage &chat_message) {
    auto json = chat_message.to_json();
    std::string message_to_send;
    message_to_send = json.dump();
    message_to_send += "\n";
    auto *stream = client_streams[client_id];
    *stream << message_to_send << std::flush;
}

void Server::send_chat_message_to_all_clients(const ChatMessage &chat_message) {
    for (std::size_t id : get_clients_id()) {
        send_chat_message(id, chat_message);
    }
}
}  // namespace meow::network