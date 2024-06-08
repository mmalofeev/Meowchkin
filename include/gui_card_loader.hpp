#ifndef GUI_CARD_LOADER_HPP_
#define GUI_CARD_LOADER_HPP_

#include <string>
#include <string_view>
#include <unordered_map>
#include "raylib-cpp.hpp"

namespace meow {

inline raylib::Image &load_card_img(std::string_view path_to_image) {
    static std::unordered_map<std::string, raylib::Image> cache;
    if (auto it = cache.find(path_to_image.data()); it != cache.end()) {
        return it->second;
    }
    return cache[path_to_image.data()] = raylib::Image(path_to_image.data()).Mipmaps();
}

}  // namespace meow

#endif  // GUI_CARD_LOADER_HPP_
