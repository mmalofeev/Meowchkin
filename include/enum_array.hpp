/*
 * motivation: std::map and std::unordered map are huge for storage of
 *             simple enum-value pairs(often there are just a few enumeration
 *             `keys`). and enums are just like numbers, but in vector/array
 *             still need static cast, actually too much of them
 * usage:      add COUNT member to your enumeration, and do not change default
 *             values
 * note:       it's just a wrapper around std::array, so i suggest to plug a
 *             pointer/reference as value(or it will allocate `COUNT` of your
 *             heavy objects!)
 */

#ifndef ENUM_ARRAY_HPP_
#define ENUM_ARRAY_HPP_

#include <array>
#include <utility>

namespace meow {

// make sure you have default enumeration and Enum::COUNT as last member!
template <typename Enum, typename T>
class EnumArray {
public:
    EnumArray() = default;

    EnumArray(std::initializer_list<std::pair<Enum, T>> &&values) {
        for (auto &[key, value] : values) {
            m_data.at(static_cast<std::size_t>(key)) = std::move(value);
        }
    }

    EnumArray(const std::initializer_list<std::pair<Enum, T>> &values) {
        for (auto &[key, value] : values) {
            m_data.at(static_cast<std::size_t>(key)) = value;
        }
    }

    T &operator[](Enum key) {
        return m_data.at(static_cast<std::size_t>(key));
    }

    const T &operator[](Enum key) const {
        return m_data.at(static_cast<std::size_t>(key));
    }

    T &operator[](std::size_t index) {
        return m_data.at(index);
    }

    const T &operator[](std::size_t index) const {
        return m_data.at(index);
    }

    [[nodiscard]] auto &data() {
        return m_data;
    }

    [[nodiscard]] const auto &data() const {
        return m_data;
    }

    [[nodiscard]] std::size_t size() const {
        return N;
    }

    EnumArray(EnumArray &&other) noexcept {
        *this = other;
    }

    EnumArray &operator=(EnumArray &&other) noexcept {
        if (this != &other) {
            m_data = std::move(other.m_data);
        }
        return *this;
    }

    EnumArray(const EnumArray &) = default;
    EnumArray &operator=(const EnumArray &) = default;
    ~EnumArray() = default;

private:
    static constexpr std::size_t N = static_cast<std::size_t>(Enum::COUNT);
    std::array<T, N> m_data;
};

}  // namespace meow

#endif  // ENUM_ARRAY_HPP_
