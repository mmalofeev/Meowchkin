#ifndef OBJECT_HPP_
#define OBJECT_HPP_
#include <cstddef>

namespace meow::model {

struct Object {
private:
    static inline std::size_t counter = 0;

public:
    const std::size_t obj_id;

    Object() : obj_id(counter++) {
    }

    Object(std::size_t _obj_id) : obj_id(_obj_id) {
    }

    virtual ~Object() = default;
};

}  // namespace meow::model

#endif