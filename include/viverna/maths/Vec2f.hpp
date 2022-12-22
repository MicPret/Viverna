#ifndef VERNA_VEC2F_HPP
#define VERNA_VEC2F_HPP

namespace verna {
struct Vec2f {
    float x;
    float y;
    constexpr Vec2f() : x(0.0f), y(0.0f) {}
    constexpr Vec2f(float x_, float y_) : x(x_), y(y_) {}
};
}  // namespace verna

#endif