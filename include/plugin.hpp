#ifndef PLUGIN_HPP_
#define PLUGIN_HPP_

#include <boost/dll/shared_library.hpp>
#include <boost/dll/shared_library_load_mode.hpp>
#include <string_view>
#include "noncopyable.hpp"

namespace meow {

template <class T>
class Plugin : Noncopyable {
private:
    boost::dll::shared_library m_library;
    T *m_ptr = nullptr;

public:
    Plugin() = default;

    Plugin(std::string_view name, std::string_view import_object) {
        reload(name, import_object);
    }

    explicit Plugin(std::pair<std::string_view, std::string_view> args) {
        reload(args.first, args.second);
    }

    void reload(std::pair<std::string_view, std::string_view> args) {
        reload(args.first, args.second);
    }

    void reload(std::string_view name, std::string_view import_object) {
        m_library.unload();
        m_ptr = nullptr;
        m_library.load(name.data(), boost::dll::load_mode::append_decorations);
        if (!m_library.has(import_object.data())) {
            throw std::runtime_error("DSo doesn't have import object!");
        }
        if (m_ptr = &m_library.get<T>(import_object.data()); m_ptr == nullptr) {
            throw std::runtime_error("invalid object in DSO!");
        }
    }

    [[nodiscard]] T *operator->() noexcept {
        return m_ptr;
    }

    [[nodiscard]] const T *operator->() const noexcept {
        return m_ptr;
    }

    [[nodiscard]] T &operator*() noexcept {
        return *m_ptr;
    }

    [[nodiscard]] const T &operator*() const noexcept {
        return *m_ptr;
    }

    [[nodiscard]] T *get() noexcept {
        return m_ptr;
    }

    [[nodiscard]] const T *get() const noexcept {
        return m_ptr;
    }
};

}  // namespace meow

#endif  // PLUGIN_HPP_
