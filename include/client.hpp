#ifndef MEOWCHKIN_CLIENT_HPP
#define MEOWCHKIN_CLIENT_HPP
#include <boost/asio.hpp>
#include <optional>
#include <queue>

#include "sended_classes.hpp"

using boost::asio::ip::tcp;

namespace meow {
class Client {
  mutable std::mutex mtx;
  boost::asio::io_context io_context;
  tcp::iostream connection;
  std::vector<PlayerInfo> players_info;
  std::size_t my_id{};
  std::string my_name;
  std::queue<Action> received_actions;
  std::queue<Feedback> received_feedbacks;
  void accept_info_about_players();

 public:
  Client();

  ~Client();

  // connect to host with input id and port in format: <id:port>
  void connect(const std::string& host);

  // disconnect from host, after it ready to connect to other port if required
  void disconnect();

  [[nodiscard]] std::size_t get_my_id()
      const;  // get id of this client given by server

  [[nodiscard]] std::string get_my_name() const;  // get name of this client

  void set_my_name(const std::string& name);

  [[nodiscard]] std::vector<PlayerInfo> get_players_info() const;

  // send to servers queued action
  void send_action(const Action& action);

  // get verified action from server from que to execute
  std::optional<Action> receive_action();

  // get feedback from que of feedback on your actions, true - valid, false -
  // invalid
  std::optional<Feedback> receive_feedback();
};
}  // namespace meow
#endif  // MEOWCHKIN_CLIENT_HPP
