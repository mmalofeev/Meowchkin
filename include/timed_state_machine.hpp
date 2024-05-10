#ifndef TIMED_STATE_MACHINE_HPP_
#define TIMED_STATE_MACHINE_HPP_

#include <chrono>
#include <concepts>
#include <stdexcept>

namespace meow {

template <typename Callback>
constexpr auto make_timed_state_machine(Callback &&callback) {
    enum State { start, event, finish };

    return [
        start_time = std::chrono::steady_clock::now(), end_time = std::chrono::steady_clock::now(),
        state = finish, callback = std::forward<Callback>(callback)
    ]<typename... Args>(std::chrono::milliseconds duration, bool invoke, Args &&...args) mutable
        requires std::invocable<
            Callback, std::chrono::steady_clock::time_point, std::chrono::steady_clock::time_point,
            Args...>
    {
        switch (state) {
            case start:
                state = event;
                start_time = std::chrono::steady_clock::now();
                end_time = start_time + duration;

            case event:
                if (std::chrono::steady_clock::now() >= end_time) {
                    state = finish;
                    return;
                }
                callback(start_time, end_time, args...);
                return;

            case finish:
                if (invoke) {
                    state = start;
                }
                return;
        }
        throw std::runtime_error("unreachable!");
    };
}

}  // namespace meow

#endif  // TIMED_STATE_MACHINE_HPP_
