// boost provides boost::noncopyable, but it is movable...

#ifndef NONCOPYABLE_HPP_
#define NONCOPYABLE_HPP_

struct Noncopyable {
    Noncopyable() = default;
    virtual ~Noncopyable() = default;
    Noncopyable(const Noncopyable &) = delete;
    Noncopyable(Noncopyable &&) = delete;
    Noncopyable &operator=(const Noncopyable &) = delete;
    Noncopyable &operator=(Noncopyable &&) = delete;
};

#endif  // NONCOPYABLE_HPP_
