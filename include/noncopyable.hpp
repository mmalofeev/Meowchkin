// boost provides boost::noncopyable, but it is movable...

#ifndef NONCOPYABLE_HPP_
#define NONCOPYABLE_HPP_

struct noncopyable {
    noncopyable() = default;
    virtual ~noncopyable() = default;
    noncopyable(const noncopyable &) = delete;
    noncopyable(noncopyable &&) = delete;
    noncopyable &operator=(const noncopyable &) = delete;
    noncopyable &operator=(noncopyable &&) = delete;
};

#endif  // NONCOPYABLE_HPP_
