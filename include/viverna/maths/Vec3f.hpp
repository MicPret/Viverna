#ifndef VERNA_VEC3F_HPP
#define VERNA_VEC3F_HPP

namespace verna {
struct Vec3f {
    float x;
    float y;
    float z;
    constexpr Vec3f() : x(0.0f), y(0.0f), z(0.0f) {}
    constexpr Vec3f(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
};
}  // namespace verna

#endif