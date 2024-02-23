#ifndef MEOWCHKIN_CLIENT_HPP
#define MEOWCHKIN_CLIENT_HPP
#include <boost/asio.hpp>
#include <optional>

#include "action.hpp"

using boost::asio::ip::tcp;

namespace meow {
class Client {
  boost::asio::io_context io_context;
  tcp::iostream connection;

 public:
  Client();

  ~Client();

  // connect to host with input id and port in format: <id:port>
  void connect(std::string host);

  // disconnect from host, after it ready to connect to other port if required
  void disconnect();

  [[nodiscard]] std::thread::id get_my_id() const;  // get id of this client given by server

  // send to servers que action
  void send_action(Action action);

  // get verified action from server from que to execute
  std::optional<Action> receive_action();

  // get feedback from que of feedback on your actions, true - valid, false - invalid
  bool get_feedback();
};
}  // namespace meow
#endif  // MEOWCHKIN_CLIENT_HPP
