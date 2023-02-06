#ifndef VERNA_VEC4F_HPP
#define VERNA_VEC4F_HPP

#include "MathUtils.hpp"
#include "Vec3f.hpp"

namespace verna {
struct Vec4f;
constexpr Vec4f operator*(float scalar, const Vec4f& vec);
constexpr Vec4f operator+(const Vec4f& a, const Vec4f& b);

struct Vec4f {
    float x;
    float y;
    float z;
    float w;
    constexpr Vec4f() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    constexpr Vec4f(float x_, float y_, float z_, float w_) :
        x(x_), y(y_), z(z_), w(w_) {}
    /**
     * @brief Initializes every component with the same value n
     *
     */
    constexpr Vec4f(float n) : x(n), y(n), z(n), w(n) {}
    constexpr float Dot(const Vec4f& other) const {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }
    constexpr float SquaredMagnitude() const { return Dot(*this); }
    constexpr float Magnitude() const {
        return maths::Sqrt(SquaredMagnitude());
    }
    constexpr float IsUnitVector() const {
        constexpr float epsilon = 2e-6;
        return maths::AreAlmostEqual(SquaredMagnitude(), 1.0f, epsilon);
    }
    constexpr Vec4f Normalized() const;
    constexpr Vec3f Xyz() const { return Vec3f(x, y, z); }
    static constexpr Vec4f UnitX() { return Vec4f(1.0f, 0.0f, 0.0f, 0.0f); }
    static constexpr Vec4f UnitY() { return Vec4f(0.0f, 1.0f, 0.0f, 0.0f); }
    static constexpr Vec4f UnitZ() { return Vec4f(0.0f, 0.0f, 1.0f, 0.0f); }
    static constexpr Vec4f UnitW() { return Vec4f(0.0f, 0.0f, 0.0f, 1.0f); }
    static constexpr Vec4f Lerp(const Vec4f& a, const Vec4f& b, float t) {
        return (1.0f - t) * a + t * b;
    }
};

constexpr Vec4f operator+(const Vec4f& a, const Vec4f& b) {
    return Vec4f(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}
constexpr Vec4f operator-(const Vec4f& a, const Vec4f& b) {
    return Vec4f(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}
constexpr Vec4f operator-(const Vec4f& vec) {
    return Vec4f(-vec.x, -vec.y, -vec.z, -vec.w);
}
constexpr Vec4f operator*(float s, const Vec4f& v) {
    return Vec4f(s * v.x, s * v.y, s * v.z, s * v.w);
}

constexpr Vec4f Vec4f::Normalized() const {
    float inverse_magnitude = 1.0f / Magnitude();
    return inverse_magnitude * (*this);
}

}  // namespace verna

#endif