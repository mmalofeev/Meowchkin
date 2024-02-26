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

struct MoveOnly {
    MoveOnly() = default;
    virtual ~MoveOnly() = default;
    MoveOnly(const MoveOnly &) = delete;
    MoveOnly &operator=(const MoveOnly &) = delete;
};

struct CopyOnly {
    CopyOnly() = default;
    virtual ~CopyOnly() = default;
    CopyOnly(CopyOnly &&) = delete;
    CopyOnly &operator=(CopyOnly &&) = delete;
};

#endif  // NONCOPYABLE_HPP_
