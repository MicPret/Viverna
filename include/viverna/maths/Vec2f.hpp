#ifndef VERNA_VEC2F_HPP
#define VERNA_VEC2F_HPP

#include "MathUtils.hpp"

namespace verna {

struct Vec2f;
constexpr Vec2f operator+(Vec2f a, Vec2f b);
constexpr Vec2f operator*(float s, Vec2f v);

struct Vec2f {
    float x;
    float y;
    constexpr Vec2f() : x(0.0f), y(0.0f) {}
    constexpr Vec2f(float x_, float y_) : x(x_), y(y_) {}
    /**
     * @brief Initializes every component with the same value n
     *
     */
    constexpr Vec2f(float n) : x(n), y(n) {}
    constexpr float Dot(Vec2f v) const { return x * v.x + y * v.y; }
    constexpr float SquaredMagnitude() const { return Dot(*this); }
    constexpr float Magnitude() const {
        return maths::Sqrt(SquaredMagnitude());
    }
    constexpr float IsUnitVector() const {
        constexpr float epsilon = 2e-6;
        return maths::AreAlmostEqual(SquaredMagnitude(), 1.0f, epsilon);
    }
    constexpr Vec2f Normalized() const;

    static constexpr Vec2f UnitX() { return Vec2f(1.0f, 0.0f); }
    static constexpr Vec2f UnitY() { return Vec2f(0.0f, 1.0f); }
    static constexpr Vec2f Lerp(const Vec2f a, const Vec2f b, float t) {
        return (1.0f - t) * a + t * b;
    }
};

constexpr Vec2f operator+(Vec2f a, Vec2f b) {
    return Vec2f(a.x + b.x, a.y + b.y);
}
constexpr Vec2f operator-(Vec2f a, Vec2f b) {
    return Vec2f(a.x - b.x, a.y - b.y);
}
constexpr Vec2f operator*(float s, Vec2f v) {
    return Vec2f(s * v.x, s * v.y);
}
constexpr Vec2f operator-(Vec2f v) {
    return Vec2f(-v.x, -v.y);
}

constexpr Vec2f Vec2f::Normalized() const {
    return (1.0f / Magnitude()) * (*this);
}
}  // namespace verna

#endif