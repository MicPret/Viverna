#ifndef VERNA_VEC2I_HPP
#define VERNA_VEC2I_HPP

#include "MathUtils.hpp"

namespace verna {

struct Vec2i;
constexpr Vec2i operator+(Vec2i a, Vec2i b);
constexpr Vec2i operator*(int s, Vec2i v);

struct Vec2i {
    int x;
    int y;
    constexpr Vec2i() : x(0), y(0) {}
    constexpr Vec2i(int x_, int y_) : x(x_), y(y_) {}
    /**
     * @brief Initializes every component with the same value n
     *
     */
    explicit constexpr Vec2i(int n) : x(n), y(n) {}
};

constexpr Vec2i operator+(Vec2i a, Vec2i b) {
    return Vec2i(a.x + b.x, a.y + b.y);
}
constexpr Vec2i operator-(Vec2i a, Vec2i b) {
    return Vec2i(a.x - b.x, a.y - b.y);
}
constexpr Vec2i operator*(int s, Vec2i v) {
    return Vec2i(s * v.x, s * v.y);
}
constexpr Vec2i operator-(Vec2i v) {
    return Vec2i(-v.x, -v.y);
}
}  // namespace verna

#endif