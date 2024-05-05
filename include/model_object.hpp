#ifndef OBJECT_HPP_
#define OBJECT_HPP_

namespace meow::model {

struct Object {
private:
    thread_local static inline std::size_t counter = 0;

public:
    const std::size_t obj_id;

    Object() : obj_id(counter++) {
    }

    Object(std::size_t _obj_id) : obj_id(_obj_id) {
    }

    static void set_seed(std::size_t seed) {
        counter = seed;
    }

    virtual ~Object() = default;
};

}  // namespace meow::model

#endif