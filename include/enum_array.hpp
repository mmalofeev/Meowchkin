/*
 * motivation: std::map and std::unordered_map are huge for storage of
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
private:
    static constexpr std::size_t N = static_cast<std::size_t>(Enum::COUNT);
    std::array<T, N> m_data;

public:
    constexpr EnumArray() = default;

    constexpr EnumArray(std::initializer_list<std::pair<Enum, T>> &&values) {
        for (auto &[key, value] : values) {
            m_data.at(static_cast<std::size_t>(key)) = std::move(value);
        }
    }

    constexpr EnumArray(const std::initializer_list<std::pair<Enum, T>> &values) {
        for (const auto &[key, value] : values) {
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

    [[nodiscard]] std::array<T, N> &data() {
        return m_data;
    }

    [[nodiscard]] const std::array<T, N> &data() const {
        return m_data;
    }

    [[nodiscard]] std::size_t size() const {
        return N;
    }

    constexpr EnumArray(EnumArray &&other) noexcept {
        *this = other;
    }

    constexpr EnumArray &operator=(EnumArray &&other) noexcept {
        if (this != &other) {
            m_data = std::move(other.m_data);
        }
        return *this;
    }

    constexpr EnumArray(const EnumArray &) noexcept = default;
    constexpr EnumArray &operator=(const EnumArray &) noexcept = default;
    constexpr ~EnumArray() = default;
};

}  // namespace meow

#endif  // ENUM_ARRAY_HPP_
