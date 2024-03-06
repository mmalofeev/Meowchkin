#ifndef MEOWCHKIN_SERVER_HPP
#define MEOWCHKIN_SERVER_HPP
#include <boost/asio.hpp>
#include <iostream>
#include <map>
#include <optional>
#include <queue>
#include <string>
#include <vector>

#include "sended_classes.hpp"

using boost::asio::ip::tcp;

namespace meow {
class Server {
  boost::asio::io_context io_context;
  tcp::acceptor acceptor;
  mutable std::mutex mtx;
  std::queue<Action> received_actions;
  std::vector<std::thread> client_threads;
  std::map<std::size_t, std::shared_ptr<tcp::iostream>> client_streams;
  std::vector<std::pair<std::size_t, std::string>> client_ids_and_names;
  void handle_client(tcp::socket&);

 public:
  Server();

  ~Server();

  // return port number for connection, get_id now is unresolved problem
  [[nodiscard]] std::string get_port() const;

  // returns ids of existed clients
  [[nodiscard]] std::vector<std::size_t> get_clients_id() const;

  // start waiting for clients in specified count, if count is wrong - now some
  // problems(i work to fix it)
  void start_listening(int num_of_clients);

  // send to client with this id action
  void send_action(std::size_t client_id, const Action& action);

  // get action from que with id of client sent it
  std::optional<Action> receive_action();

  // send feedback on client action, true - valid, false - invalid
  void send_feedback(std::size_t client_id, const Feedback&);

  void send_action_to_all_clients(const Action& action);
};
}  // namespace meow
#endif  // MEOWCHKIN_SERVER_HPP
