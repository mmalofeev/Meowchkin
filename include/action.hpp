#ifndef MEOWCHKIN_ACTION_HPP
#define MEOWCHKIN_ACTION_HPP

enum class some_commands {};  // will be specified in future

namespace meow {
class Action {
 public:
  std::vector<some_commands> commands;
  std::optional<std::thread::id> target;
};
}  // namespace meow

#endif  // MEOWCHKIN_ACTION_HPP
