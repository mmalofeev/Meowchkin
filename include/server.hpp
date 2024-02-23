#ifndef MEOWCHKIN_SERVER_HPP
#define MEOWCHKIN_SERVER_HPP
#include <boost/asio.hpp>
#include <iostream>
#include <optional>
#include <queue>
#include <string>
#include <vector>

#include "action.hpp"

using boost::asio::ip::tcp;

namespace meow {
class Server {
  boost::asio::io_context io_context;
  tcp::acceptor acceptor;
  std::queue<std::string> received_messages;
  std::mutex mtx;
  std::vector<std::thread> client_threads;
  void handle_client(tcp::socket&);

 public:
  Server();

  ~Server();

  // return port number for connection, get_id now is unresolved problem
  [[nodiscard]] std::string get_port() const;

  // returns ids of existed clients
  [[nodiscard]] std::vector<std::thread::id> get_clients_id() const;

  // start waiting for clients in specified count, if count is wrong - now some
  // problems(i work to fix it)
  void start_accepting(int client_count);

  // send to client with this id action
  void send_action(std::thread::id client_id, Action action);

  // get action from que with id of client sent it
  std::optional<std::pair<std::thread::id, Action>> receive_action();

  // send feedback on client action, true - valid, false - invalid
  void send_feedback(std::thread::id client_id, bool feedback);
};
}  // namespace meow
#endif  // MEOWCHKIN_SERVER_HPP
