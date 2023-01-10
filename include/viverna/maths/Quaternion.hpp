#ifndef VERNA_QUATERNION_HPP
#define VERNA_QUATERNION_HPP

#include "Vec3f.hpp"

namespace verna {
/**
 * @brief Unit-length quaternions are used to represent spatial rotations
 *
 */
struct Quaternion {
    float x;
    float y;
    float z;
    float w;

    constexpr Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
    constexpr Quaternion(float x_, float y_, float z_, float w_)
        : x(x_), y(y_), z(z_), w(w_) {}
    Quaternion(const Vec3f& unit_axis, float radians);

    constexpr float SquaredMagnitude() const {
        return x * x + y * y + z * z + w * w;
    }

    /**
     * @brief In case of unit-length quaternions, Conjugate() computes the
     * inverse quaternion
     *
     * @return The conjugate quaternion, which happens to be the inverse for
     * unit-length quaternions
     */
    constexpr Quaternion Conjugate() const { return Quaternion(-x, -y, -z, w); }

    /**
     * @brief Use Conjugate() when dealing with unit-length quaternions instead
     *
     * @return The inverse quaternion
     */
    constexpr Quaternion Inverse() const {
        float ism = 1.0f / SquaredMagnitude();  // inverse squared magnitude
        Quaternion conj = Conjugate();
        return Quaternion(conj.x * ism, conj.y * ism, conj.z * ism,
                          conj.w * ism);
    }

    /**
     * @brief Returns true for quaternions that can be used to represent spatial
     * rotations
     *
     * @return Whether the quaternion is a unit-length one
     */
    constexpr bool IsUnitLength() const {
        constexpr float epsilon = 2e-6;
        return maths::AreAlmostEqual(SquaredMagnitude(), 1.0f, epsilon);
    }

    /**
     * @brief Returns a rotated vector. WARNING: Rotate() assumes this is a
     * unit-length quaternion
     *
     * @param vec vector to rotate
     * @return The rotated vector
     */
    constexpr Vec3f Rotate(const Vec3f& vec) const;
};

/**
 * @brief Computes the result of rotation p applied after rotation q
 *
 * @param p second rotation
 * @param q first rotation
 * @return constexpr Quaternion
 */
constexpr Quaternion operator*(const Quaternion& p, const Quaternion& q) {
    Vec3f vp(p.x, p.y, p.z);
    Vec3f vq(q.x, q.y, q.z);
    Vec3f vec = p.w * vq + q.w * vp + vp.Cross(vq);
    return Quaternion(vec.x, vec.y, vec.z, p.w * q.w - vp.Dot(vq));
}

constexpr Vec3f Quaternion::Rotate(const Vec3f& vec) const {
    Quaternion v(vec.x, vec.y, vec.z, 0.0f);
    v = *this * v * Conjugate();
    return Vec3f(v.x, v.y, v.z);
}
}  // namespace verna

#endif