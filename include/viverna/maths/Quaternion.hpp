#ifndef VERNA_QUATERNION_HPP
#define VERNA_QUATERNION_HPP

#include "Vec3f.hpp"
#include "Mat4f.hpp"

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
    constexpr Quaternion(float x_, float y_, float z_, float w_) :
        x(x_), y(y_), z(z_), w(w_) {}
    Quaternion(const Vec3f& unit_axis, float radians);
    Quaternion(const Mat4f& rot_matrix);

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

    /**
     * @brief Computes quaternion to rotation matrix conversion, assuming it's a
     * unit quaternion
     *
     * @return Equivalent rotation matrix
     */
    constexpr Mat4f AsMatrix() const {
        Mat4f r;
        r[0] = 1.0f - 2.0f * (y * y + z * z);
        r[1] = 2.0f * (x * y + w * z);
        r[2] = 2.0f * (x * z - w * y);
        r[4] = 2.0f * (x * y - w * z);
        r[5] = 1.0f - 2.0f * (x * x + z * z);
        r[6] = 2.0f * (y * z + w * x);
        r[8] = 2.0f * (x * z + w * y);
        r[9] = 2.0f * (y * z - w * x);
        r[10] = 1.0f - 2.0f * (x * x + y * y);
        r[15] = 1.0f;
        return r;
    }

    static constexpr Quaternion Lerp(const Quaternion& a,
                                     const Quaternion& b,
                                     float t) {
        float t_m = 1.0f - t;
        return Quaternion(t_m * a.x + t * b.x, t_m * a.y + t * b.y,
                          t_m * a.z + t_m * b.z, t_m * a.w + t * b.w);
    }
};

/**
 * @brief Computes the result of rotation p applied after rotation q
 *
 * @param p second rotation
 * @param q first rotation
 * @return The Hamilton product of two quaternions
 */
constexpr Quaternion operator*(const Quaternion& p, const Quaternion& q) {
    float w = p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z;
    float x = p.w * q.x + p.x * q.w + p.y * q.z - p.z * q.y;
    float y = p.w * q.y - p.x * q.z + p.y * q.w + p.z * q.x;
    float z = p.w * q.z + p.x * q.y - p.y * q.x + p.z * q.w;
    return Quaternion(x, y, z, w);
}

constexpr Vec3f Quaternion::Rotate(const Vec3f& vec) const {
    Quaternion v(vec.x, vec.y, vec.z, 0.0f);
    v = *this * v * Conjugate();
    return Vec3f(v.x, v.y, v.z);
}
}  // namespace verna

#endif