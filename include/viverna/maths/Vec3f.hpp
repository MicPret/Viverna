#ifndef VERNA_VEC3F_HPP
#define VERNA_VEC3F_HPP

#include "MathUtils.hpp"
#include "Vec2f.hpp"

namespace verna {
struct Vec3f;
constexpr Vec3f operator*(float scalar, const Vec3f& vector);
constexpr Vec3f operator+(const Vec3f& a, const Vec3f& b);
constexpr Vec3f operator-(const Vec3f& a, const Vec3f& b);

struct Vec3f {
    float x;
    float y;
    float z;
    constexpr Vec3f() : x(0.0f), y(0.0f), z(0.0f) {}
    constexpr Vec3f(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    /**
     * @brief Initializes every component with the same value n
     *
     */
    explicit constexpr Vec3f(float n) : x(n), y(n), z(n) {}
    constexpr float Dot(const Vec3f& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    constexpr Vec3f Cross(const Vec3f& other) const {
        return Vec3f(y * other.z - other.y * z, z * other.x - other.z * x,
                     x * other.y - other.x * y);
    }
    /**
     * @brief Returns a vector made from a componentwise multiplication
     *
     * @param other Input vector
     * @return Componentwise product
     */
    constexpr Vec3f ComponentProduct(const Vec3f& other) const {
        return Vec3f(x * other.x, y * other.y, z * other.z);
    }
    constexpr float SquaredMagnitude() const { return Dot(*this); }
    constexpr float Magnitude() const {
        return maths::Sqrt(SquaredMagnitude());
    }
    constexpr float IsUnitVector() const {
        constexpr float epsilon = 2e-6f;
        return maths::AreAlmostEqual(Magnitude(), 1.0f, epsilon);
    }
    constexpr Vec3f Normalized() const;
    constexpr Vec2f Xy() const { return Vec2f(x, y); }
    constexpr bool IsAlmostEqual(const Vec3f& other,
                                 float epsilon = 2e-6f) const {
        Vec3f diff = *this - other;
        float sum =
            maths::Abs(diff.x) + maths::Abs(diff.y) + maths::Abs(diff.z);
        return sum < epsilon;
    }

    static constexpr Vec3f UnitX() { return Vec3f(1.0f, 0.0f, 0.0f); }
    static constexpr Vec3f UnitY() { return Vec3f(0.0f, 1.0f, 0.0f); }
    static constexpr Vec3f UnitZ() { return Vec3f(0.0f, 0.0f, 1.0f); }
    static constexpr Vec3f Lerp(const Vec3f& a, const Vec3f& b, float t) {
        return (1.0f - t) * a + t * b;
    }
    /**
     * @brief Returns a vector made from the smallest components of the two
     *
     * @param a First vector
     * @param b Second vector
     * @return {0.0f, 1.0f, 2.0f), {1.0f, 1.0f, -1.0f} => {0.0f, 1.0f, -1.0f}
     */
    static constexpr Vec3f Min(const Vec3f& a, const Vec3f& b) {
        return Vec3f(maths::Min(a.x, b.x), maths::Min(a.y, b.y),
                     maths::Min(a.z, b.z));
    }
    /**
     * @brief Returns a vector made from the largest components of the two
     *
     * @param a First vector
     * @param b Second vector
     * @return {0.0f, 1.0f, 2.0f), {1.0f, 1.0f, -1.0f} => {1.0f, 1.0f, 2.0f}
     */
    static constexpr Vec3f Max(const Vec3f& a, const Vec3f& b) {
        return Vec3f(maths::Max(a.x, b.x), maths::Max(a.y, b.y),
                     maths::Max(a.z, b.z));
    }
    /**
     * @brief Computes a random unit vector
     *
     * @return A random unit vector
     */
    static Vec3f Random();

    /**
     * @brief Computes a vector from polar coordinates
     *
     * @param azimuth Horizontal clockwise rotation on the ZX plane (starting
     * from Z) in radians
     * @param zenit Vertical clockwise rotation on the YZ plane (starting from
     * Y) in radians
     * @param magnitude Desired length of the vector
     * @return Vector in Cartesian coordinates
     */
    static Vec3f FromPolarCoordinates(float azimuth,
                                      float zenit,
                                      float magnitude = 1.0f);
};

constexpr Vec3f operator*(float scalar, const Vec3f& vector) {
    return Vec3f(scalar * vector.x, scalar * vector.y, scalar * vector.z);
}
constexpr Vec3f operator+(const Vec3f& a, const Vec3f& b) {
    return Vec3f(a.x + b.x, a.y + b.y, a.z + b.z);
}
constexpr Vec3f operator-(const Vec3f& a, const Vec3f& b) {
    return Vec3f(a.x - b.x, a.y - b.y, a.z - b.z);
}
constexpr Vec3f operator-(const Vec3f& vector) {
    return Vec3f(-vector.x, -vector.y, -vector.z);
}
constexpr void operator+=(Vec3f& a, const Vec3f& b) {
    a = a + b;
}
constexpr void operator-=(Vec3f& a, const Vec3f& b) {
    a = a - b;
}

constexpr Vec3f Vec3f::Normalized() const {
    float inverse_magnitude = 1.0f / Magnitude();
    return inverse_magnitude * (*this);
}
}  // namespace verna

#endif