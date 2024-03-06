#include "server.hpp"

#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include <optional>

using boost::asio::ip::tcp;

namespace meow {
Server::Server()
    : io_context(), acceptor(io_context, tcp::endpoint(tcp::v4(), 0)) {}

Server::~Server() {
  for (auto& thread : client_threads) {
    thread.join();
  }
}

[[nodiscard]] std::string Server::get_port() const {
  return std::to_string(acceptor.local_endpoint().port());
}

[[nodiscard]] std::vector<std::size_t> Server::get_clients_id() const {
  std::vector<std::size_t> clients_id;
  clients_id.reserve(client_threads.size());
  for (auto& pair : client_ids_and_names) {
    clients_id.push_back(pair.first);
  }
  return clients_id;
}

void Server::handle_client(tcp::socket& socket) {
  //  tcp::iostream client(std::move(socket));
  std::shared_ptr<tcp::iostream> client(new tcp::iostream(std::move(socket)));
  std::size_t client_id =
      std::hash<std::thread::id>{}(std::this_thread::get_id());
  std::string client_name;
  std::getline(*client, client_name);

  mtx.lock();
  client_ids_and_names.emplace_back(client_id, client_name);
  client_streams[client_id] = client;
  mtx.unlock();

  while (client) {
    std::string received_msg;
    if (!std::getline(*client, received_msg)) {
      break;
    }
    auto json = json::parse(received_msg);
    std::unique_lock l(mtx);
    received_actions.emplace(json);
  }
}

void Server::start_listening(int num_of_clients) {
  int count_of_accepted_clients = 0;
  while (count_of_accepted_clients < num_of_clients) {
    tcp::socket socket = acceptor.accept();
    std::thread new_thread =
        std::thread([&socket, this]() { handle_client(socket); });
    new_thread.detach();
    client_threads.push_back(std::move(new_thread));
    ++count_of_accepted_clients;
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