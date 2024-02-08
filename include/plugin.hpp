#ifndef PLUGIN_HPP_
#define PLUGIN_HPP_

#include <boost/dll/shared_library.hpp>
#include <boost/dll/shared_library_load_mode.hpp>
#include <string_view>
#include "noncopyable.hpp"

namespace meow {

template <class T>
class plugin : noncopyable {
private:
    T *m_ptr = nullptr;
    boost::dll::shared_library m_library;

public:
    explicit plugin() = default;

    explicit plugin(const std::string_view &name, const std::string_view &import_item) {
        reload(name, import_item);
    }

    explicit plugin(const std::pair<const char *, const char *> &name_item) {
        reload(name_item.first, name_item.second);
    }

    void reload(const std::pair<const char *, const char *> &name_item) {
        reload(name_item.first, name_item.second);
    }

    void reload(std::string_view name, std::string_view import_item) {
        // std::cerr << "reloading " << name << '\n';
        m_library.unload();
        m_library.load(name, boost::dll::load_mode::append_decorations);
        if (!m_library.has(import_item.data())) {
            throw std::runtime_error("library doesn't have import items.");
        }
        assert(m_ptr = &m_library.get<T>(import_item.data()));
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
