#ifndef MEOWCHKIN_CLIENT_HPP
#define MEOWCHKIN_CLIENT_HPP
#include <boost/asio.hpp>
#include <optional>
#include <queue>

#include "message_types.hpp"

using boost::asio::ip::tcp;

namespace meow::network {
class Client {
  mutable std::mutex mtx;
  boost::asio::io_context io_context;
  tcp::iostream connection;
  std::vector<PlayerInfo> players_info;
  std::size_t id_of_client{};
  std::string name_of_client;
  std::queue<Action> received_actions;
  std::queue<ActionResult> received_action_results;
  void accept_info_about_players();

 public:
  Client();

  ~Client();

  // connect to host with input id and port in format: <id:port>
  void connect(const std::string& host);

  // disconnect from host, after it ready to connect to other port if required
  void disconnect();

  [[nodiscard]] std::size_t get_id_of_client()
      const;  // get id of this client given by server

  [[nodiscard]] std::string get_name_of_client()
      const;  // get name of this client

  void set_name_of_client(const std::string& name);

  [[nodiscard]] const std::vector<PlayerInfo>& get_players_info() const;

  // send to servers queued action
  void send_action(const Action& action);

  // get verified action from server from queue to execute
  std::optional<Action> receive_action();

  // get ActionResult from queue of ActionResults on your actions, true - valid,
  // false - invalid
  std::optional<ActionResult> receive_action_result();
};
}  // namespace meow::network
#endif  // MEOWCHKIN_CLIENT_HPP
