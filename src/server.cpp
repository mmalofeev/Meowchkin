#include "server.hpp"

#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include <optional>

using boost::asio::ip::tcp;

namespace meow {
Server::Server()
    : io_context(), acceptor(io_context, tcp::endpoint(tcp::v4(), 0)) {}

Server::~Server() = default;

[[nodiscard]] std::string Server::get_port() const {
  return std::to_string(acceptor.local_endpoint().port());
}

[[nodiscard]] std::vector<std::size_t> Server::get_clients_id() const {
  std::vector<std::size_t> clients_id;
  clients_id.reserve(client_threads.size());
  for (auto& info : players_info) {
    clients_id.push_back(info.id);
  }
  return clients_id;
}

void Server::handle_client(tcp::socket& socket) {
  tcp::iostream client(std::move(socket));
  std::size_t client_id =
      std::hash<std::thread::id>{}(std::this_thread::get_id());
  std::string client_name;
  client >> client_name;

  mtx.lock();
  players_info.emplace_back(client_id, client_name);
  client_streams[client_id] = &client;
  ++count_of_handled_clients;
  mtx.unlock();

  while (client) {
    std::string received_msg;
    if (!std::getline(client, received_msg)) {
      break;
    }
    if (received_msg.empty()) continue;
    auto json = json::parse(received_msg);
    std::unique_lock l(mtx);
    received_actions.emplace(json);
  }
}

void Server::send_players_info(std::size_t client_id) {
  std::vector<std::string> messages;
  for (auto& info : players_info) {
    auto json = info.to_json();
    std::string msg = json.dump();
    msg += "\n";
    messages.push_back(msg);
  }
  auto stream = client_streams[client_id];
  *stream << messages.size() << '\n' << std::flush;
  *stream << client_id << '\n' << std::flush;
  for (std::string& msg : messages) {
    *stream << msg << std::flush;
  }
}

void Server::start_listening(int num_of_clients) {
  int count_of_accepted_clients = 0;
  while (count_of_accepted_clients < num_of_clients) {
    tcp::socket socket = acceptor.accept();
    std::thread new_thread = std::thread(
        [s = std::move(socket), this]() mutable { handle_client(s); });
    new_thread.detach();
    client_threads.push_back(std::move(new_thread));
    ++count_of_accepted_clients;
  }
  while (count_of_handled_clients < num_of_clients) {
    sleep(1);
  }
  for (std::size_t id : get_clients_id()) {
    send_players_info(id);
  }
}

void Server::send_action(std::size_t client_id, const Action& action) {
  auto json = action.to_json();
  std::string message_to_send;
  message_to_send = json.dump();
  message_to_send += "\n";
  auto stream = client_streams[client_id];
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

void Server::send_feedback(std::size_t client_id, const Feedback& feedback) {
  auto json = feedback.to_json();
  std::string message_to_send;
  message_to_send = json.dump();
  message_to_send += '\n';
  auto stream = client_streams[client_id];
  *stream << message_to_send << std::flush;
}

void Server::send_action_to_all_clients(const Action& action) {
  for (std::size_t id : get_clients_id()) {
    send_action(id, action);
  }
}
}  // namespace meow