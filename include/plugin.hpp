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
    T *m_ptr = nullptr;
    boost::dll::shared_library m_library;

public:
    explicit Plugin() = default;

    explicit Plugin(std::string_view name, std::string_view import_item) {
        reload(name.data(), import_item.data());
    }

    explicit Plugin(const std::pair<std::string_view, std::string_view> &name_item) {
        reload(name_item.first.data(), name_item.second.data());
    }

    void reload(const std::pair<std::string_view, std::string_view> &name_item) {
        reload(name_item.first.data(), name_item.second.data());
    }

    void reload(std::string_view name, std::string_view import_item) {
        m_library.unload();
        m_ptr = nullptr;
        m_library.load(name.data(), boost::dll::load_mode::append_decorations);
        if (!m_library.has(import_item.data())) {
            throw std::runtime_error("DSo doesn't have import items!");
        }
        if (m_ptr = &m_library.get<T>(import_item.data()); !m_ptr) {
            throw std::runtime_error("invalid object in DSo!");
        }
    }

    [[nodiscard]] T *operator->() noexcept {
        return m_ptr;
    }

    [[nodiscard]] T *operator*() noexcept {
        return m_ptr;
    }

    [[nodiscard]] T *get() noexcept {
        return m_ptr;
    }
};

}  // namespace meow

#endif  // PLUGIN_HPP_
